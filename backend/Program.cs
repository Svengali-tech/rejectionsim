// Rejection Simulator 3000 -- Global Leaderboard Backend
// .NET 8 Minimal API
//
// Endpoints:
//   POST /scores        -- submit a run
//   GET  /scores        -- get top 100 runs
//   GET  /scores/stats  -- aggregate stats (total apps submitted, avg offers, etc.)
//
// Run: dotnet run
// Default port: 5000
//
// Storage: SQLite via EF Core so there are zero external dependencies.
// Swap the connection string in appsettings.json to use Postgres in prod.

using Microsoft.EntityFrameworkCore;
using System.ComponentModel.DataAnnotations;

var builder = WebApplication.CreateBuilder(args);

// Register SQLite database context
builder.Services.AddDbContext<LeaderboardDb>(opt =>
    opt.UseSqlite(builder.Configuration.GetConnectionString("Default")
        ?? "Data Source=leaderboard.db"));

var app = builder.Build();

// Migrate on startup so the DB is ready without manual steps
using (var scope = app.Services.CreateScope()) {
    var db = scope.ServiceProvider.GetRequiredService<LeaderboardDb>();
    db.Database.EnsureCreated();
}

// ---- POST /scores ---------------------------------------------------------------
// Submit a completed run. Validates input and stores it.
// The C++ client POSTs JSON after [q]uit or [r]eset.
app.MapPost("/scores", async (ScoreSubmission submission, LeaderboardDb db) => {
    // Basic validation
    if (submission.Applied < 0 || submission.Offers < 0 || submission.Prestige < 0)
        return Results.BadRequest("Negative values not allowed.");

    if (submission.Applied > 10000)
        return Results.BadRequest("Applied count suspiciously high.");

    if (string.IsNullOrWhiteSpace(submission.Alias))
        submission.Alias = "Anonymous";

    // Sanitize alias -- no HTML, max 20 chars
    submission.Alias = submission.Alias.Trim();
    if (submission.Alias.Length > 20)
        submission.Alias = submission.Alias[..20];

    var entry = new ScoreEntry {
        Alias        = submission.Alias,
        Applied      = submission.Applied,
        Rejected     = submission.Rejected,
        Ghosted      = submission.Ghosted,
        Interviews   = submission.Interviews,
        Offers       = submission.Offers,
        Prestige     = submission.Prestige,
        FastestOffer = submission.FastestOffer,
        DaysPlayed   = submission.DaysPlayed,
        Track        = submission.Track ?? "SWE",
        Difficulty   = submission.Difficulty ?? "Normal",
        HopeLeft     = submission.HopeLeft,
        SubmittedAt  = DateTime.UtcNow
    };

    db.Scores.Add(entry);
    await db.SaveChangesAsync();

    return Results.Created($"/scores/{entry.Id}", new { entry.Id, Message = "Submitted." });
});

// ---- GET /scores ---------------------------------------------------------------
// Returns top 100 runs sorted by offers desc, then fastest offer asc, then fewest rejections.
// Query params: track=SWE|Finance|Consulting, difficulty=Easy|Normal|Hard|Nightmare
app.MapGet("/scores", async (
    LeaderboardDb db,
    string? track,
    string? difficulty,
    int page = 1,
    int pageSize = 25) =>
{
    pageSize = Math.Clamp(pageSize, 1, 100);
    page     = Math.Max(1, page);

    var query = db.Scores.AsQueryable();

    if (!string.IsNullOrEmpty(track))
        query = query.Where(s => s.Track == track);

    if (!string.IsNullOrEmpty(difficulty))
        query = query.Where(s => s.Difficulty == difficulty);

    // Primary sort: most offers. Secondary: fastest offer (day number, lower = better).
    // Tertiary: fewest rejections.
    var results = await query
        .OrderByDescending(s => s.Offers)
        .ThenBy(s => s.FastestOffer < 0 ? int.MaxValue : s.FastestOffer)
        .ThenBy(s => s.Rejected)
        .Skip((page - 1) * pageSize)
        .Take(pageSize)
        .Select(s => new {
            s.Id,
            s.Alias,
            s.Applied,
            s.Rejected,
            s.Offers,
            s.Prestige,
            s.FastestOffer,
            s.DaysPlayed,
            s.Track,
            s.Difficulty,
            s.HopeLeft,
            s.SubmittedAt
        })
        .ToListAsync();

    var total = await query.CountAsync();

    return Results.Ok(new {
        Page     = page,
        PageSize = pageSize,
        Total    = total,
        Results  = results
    });
});

// ---- GET /scores/stats ---------------------------------------------------------
// Aggregate stats across all submitted runs.
app.MapGet("/scores/stats", async (LeaderboardDb db) => {
    var count = await db.Scores.CountAsync();
    if (count == 0)
        return Results.Ok(new { Message = "No runs submitted yet." });

    var stats = await db.Scores.GroupBy(_ => 1).Select(g => new {
        TotalRuns         = g.Count(),
        TotalApps         = g.Sum(s => s.Applied),
        TotalOffers       = g.Sum(s => s.Offers),
        AvgAppsPerRun     = g.Average(s => s.Applied),
        AvgOffersPerRun   = g.Average(s => s.Offers),
        AvgHopeLeft       = g.Average(s => s.HopeLeft),
        MaxPrestige       = g.Max(s => s.Prestige),
        FastestOfferEver  = g.Where(s => s.FastestOffer > 0).Min(s => (int?)s.FastestOffer),
        MostAppsInOneRun  = g.Max(s => s.Applied),
    }).FirstAsync();

    return Results.Ok(stats);
});

// ---- GET /scores/{id} ----------------------------------------------------------
// Fetch a single run by ID.
app.MapGet("/scores/{id:int}", async (int id, LeaderboardDb db) => {
    var entry = await db.Scores.FindAsync(id);
    return entry is null ? Results.NotFound() : Results.Ok(entry);
});

app.Run();

// ---- DATA MODELS ---------------------------------------------------------------

// The EF Core database context. One table: Scores.
class LeaderboardDb(DbContextOptions<LeaderboardDb> options) : DbContext(options) {
    public DbSet<ScoreEntry> Scores => Set<ScoreEntry>();
}

// Stored in the database. ID is auto-assigned.
class ScoreEntry {
    public int      Id           { get; set; }
    [MaxLength(20)]
    public string   Alias        { get; set; } = "Anonymous";
    public int      Applied      { get; set; }
    public int      Rejected     { get; set; }
    public int      Ghosted      { get; set; }
    public int      Interviews   { get; set; }
    public int      Offers       { get; set; }
    public int      Prestige     { get; set; }
    public int      FastestOffer { get; set; }  // -1 = never got one
    public int      DaysPlayed   { get; set; }
    [MaxLength(20)]
    public string   Track        { get; set; } = "SWE";
    [MaxLength(20)]
    public string   Difficulty   { get; set; } = "Normal";
    public int      HopeLeft     { get; set; }
    public DateTime SubmittedAt  { get; set; }
}

// What the C++ client sends in the POST body.
// Mirrors the fields in saveload.h so serialization is straightforward.
record ScoreSubmission(
    string? Alias,
    int     Applied,
    int     Rejected,
    int     Ghosted,
    int     Interviews,
    int     Offers,
    int     Prestige,
    int     FastestOffer,
    int     DaysPlayed,
    string? Track,
    string? Difficulty,
    int     HopeLeft
);
