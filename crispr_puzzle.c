/*
 * GeneCraft - Module 1: CRISPR-inspired DNA editing puzzle
 * Simulation only: DNA is a C string, edits are string operations.
 *
 * Compile:  gcc crispr_puzzle.c -o crispr_puzzle
 * Run:      ./crispr_puzzle        (Windows: crispr_puzzle.exe)
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LEN 64

typedef struct {
    const char *start;   /* faulty DNA */
    const char *target;  /* healthy DNA */
    int maxMoves;        /* move budget */
    const char *hint;
} Level;

static const Level LEVELS[] = {
    {"ATGCCG",   "ATGGCG",     2, "One base is wrong. Try REPLACE."},
    {"ATGGGCG",  "ATGGCG",     2, "There is an extra base. Try CUT."},
    {"ATGCG",    "ATGACG",     2, "A base is missing. Try INSERT."},
    {"TTGACCTA", "ATGCCTA",    3, "One wrong base and one extra base."},
    {"ATGCCATG", "ATGGCCATGA", 3, "Two bases are missing."}
};
#define NUM_LEVELS ((int)(sizeof(LEVELS) / sizeof(LEVELS[0])))

/* ---------- helpers ---------- */

int isValidBase(char b)
{
    return b == 'A' || b == 'T' || b == 'G' || b == 'C';
}

/* Read one line safely. Returns 0 on end of input. */
int readLine(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL) return 0;
    buf[strcspn(buf, "\n")] = '\0';
    return 1;
}

/* ---------- the three edit operations (pos is 1-based) ---------- */

void replaceBase(char *dna, int pos, char base)
{
    dna[pos - 1] = base;
}

/* Cut: shift everything after the position one step LEFT */
void cutBase(char *dna, int pos)
{
    for (int i = pos - 1; dna[i] != '\0'; i++)
        dna[i] = dna[i + 1];
}

/* Insert: shift everything from the end one step RIGHT, then fill the gap */
void insertBase(char *dna, int pos, char base)
{
    int len = (int)strlen(dna);
    for (int i = len; i >= pos - 1; i--)
        dna[i + 1] = dna[i];
    dna[pos - 1] = base;
}

/* ---------- display ---------- */

void showSequences(const char *cur, const char *target)
{
    int n = (int)strlen(cur), m = (int)strlen(target);
    int max = n > m ? n : m;

    printf("\n  Pos    : ");
    for (int i = 0; i < max; i++) printf("%-3d", i + 1);
    printf("\n  Current: ");
    for (int i = 0; i < n; i++) printf("%-3c", cur[i]);
    printf("\n  Target : ");
    for (int i = 0; i < m; i++) printf("%-3c", target[i]);
    printf("\n\n");
}

/* ---------- one level ---------- */

/* Returns 1 if won, 0 if lost or given up */
int playLevel(int index)
{
    const Level *lv = &LEVELS[index];
    char dna[MAX_LEN];
    char line[128];
    int moves = 0;

    strcpy(dna, lv->start);

    printf("\n==============================\n");
    printf(" LEVEL %d of %d\n", index + 1, NUM_LEVELS);
    printf(" Hint: %s\n", lv->hint);
    printf("==============================\n");

    while (moves < lv->maxMoves) {
        printf("Moves used: %d / %d", moves, lv->maxMoves);
        showSequences(dna, lv->target);

        printf("1) Replace   2) Cut   3) Insert   0) Give up\nChoose: ");
        if (!readLine(line, sizeof line)) return 0;

        int choice;
        if (sscanf(line, "%d", &choice) != 1 || choice < 0 || choice > 3) {
            printf("Please enter 0, 1, 2 or 3.\n");
            continue;                       /* not counted as a move */
        }
        if (choice == 0) return 0;

        int len = (int)strlen(dna);
        int pos;
        char base = 0;

        printf("Position (1-%d): ", choice == 3 ? len + 1 : len);
        if (!readLine(line, sizeof line)) return 0;
        if (sscanf(line, "%d", &pos) != 1) {
            printf("That is not a number.\n");
            continue;
        }

        /* range checks */
        if (choice == 3) {
            if (pos < 1 || pos > len + 1) { printf("Position out of range.\n"); continue; }
            if (len >= MAX_LEN - 1)       { printf("Sequence is too long.\n");  continue; }
        } else {
            if (pos < 1 || pos > len)     { printf("Position out of range.\n"); continue; }
        }

        /* base needed for replace and insert */
        if (choice == 1 || choice == 3) {
            printf("Base (A/T/G/C): ");
            if (!readLine(line, sizeof line)) return 0;
            base = (char)toupper((unsigned char)line[0]);
            if (!isValidBase(base)) {
                printf("Invalid base. Use A, T, G or C.\n");
                continue;
            }
        }

        /* perform the edit */
        if (choice == 1)      replaceBase(dna, pos, base);
        else if (choice == 2) cutBase(dna, pos);
        else                  insertBase(dna, pos, base);
        moves++;

        /* check for a win */
        if (strcmp(dna, lv->target) == 0) {
            showSequences(dna, lv->target);
            printf("*** LEVEL COMPLETE! Fixed in %d move(s). ***\n", moves);
            return 1;
        }
    }

    printf("\nOut of moves. Final: %s  Target: %s\n", dna, lv->target);
    return 0;
}

/* ---------- main ---------- */

int main(void)
{
    char line[64];
    int won = 0;

    printf("GeneCraft - CRISPR Puzzle\n");
    printf("Edit the faulty DNA until it matches the healthy target.\n");
    printf("(Simulation only - this is not real laboratory CRISPR.)\n");

    for (int i = 0; i < NUM_LEVELS; i++) {
        int ok = playLevel(i);
        while (!ok) {
            printf("\nTry this level again? (y/n): ");
            if (!readLine(line, sizeof line) || tolower((unsigned char)line[0]) != 'y')
                goto done;
            ok = playLevel(i);
        }
        won++;
    }
done:
    printf("\nLevels completed: %d / %d\nThanks for playing GeneCraft!\n", won, NUM_LEVELS);
    return 0;
}
