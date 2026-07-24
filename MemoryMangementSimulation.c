#include <stdio.h>

void displayFrames(int frames[], int frameSize)
{
    printf("Frames: ");
    for (int i = 0; i < frameSize; i++)
    {
        if (frames[i] == -1)
            printf("- "); // Empty frame
        else
            printf("%d ", frames[i]); // Frame currently holding this page
    }
    printf("\n");
}

// FIFO Page Replacement
void FIFO(int pages[], int n, int frameSize)
{
    int frames[frameSize];       // Simulated physical memory frames
    int front = 0;                // Points to the oldest loaded page (next to be replaced)
    int hits = 0, faults = 0;     // Performance counters

    // Initialize all frames as empty
    for (int i = 0; i < frameSize; i++)
        frames[i] = -1;

    printf("\n===== FIFO =====\n");

    // Process each page reference in order
    for (int i = 0; i < n; i++)
    {
        int found = 0;

        // Check if the requested page is already loaded in any frame
        for (int j = 0; j < frameSize; j++)
        {
            if (frames[j] == pages[i])
            {
                found = 1;
                break;
            }
        }

        if (found)
        {
            // Page already in memory: no replacement needed
            hits++;
            printf("Page %d -> Hit\n", pages[i]);
        }
        else
        {
            // Page fault: load the page at the current 'front' position,
            // overwriting whichever page has been resident the longest
            faults++;
            frames[front] = pages[i];
            front = (front + 1) % frameSize; // Move to next oldest slot (circular)
            printf("Page %d -> Fault\n", pages[i]);
        }

        displayFrames(frames, frameSize); // Show frame state after this reference
    }

    // Print summary statistics
    printf("\nHits       : %d\n", hits);
    printf("Faults     : %d\n", faults);
    printf("Hit Ratio  : %.2f\n", (float)hits / n);
    printf("Miss Ratio : %.2f\n", (float)faults / n);
}


// LRU (Least Recently Used) Page Replacement
void LRU(int pages[], int n, int frameSize)
{
    int frames[frameSize];  // Simulated physical memory frames
    int recent[frameSize];  // Tracks the last-used tick for each frame
    int hits = 0, faults = 0;

    // Initialize frames as empty and recency values as unused
    for (int i = 0; i < frameSize; i++)
    {
        frames[i] = -1;
        recent[i] = -1;
    }

    printf("\n===== LRU =====\n");

    for (int i = 0; i < n; i++)
    {
        int found = -1;

        // Check if the requested page is already loaded, and if so, which frame
        for (int j = 0; j < frameSize; j++)
        {
            if (frames[j] == pages[i])
            {
                found = j;
                break;
            }
        }

        if (found != -1)
        {
            // Page already in memory: update its "last used" tick
            hits++;
            recent[found] = i;
            printf("Page %d -> Hit\n", pages[i]);
        }
        else
        {
            // Page fault: find the frame that was used least recently
            faults++;
            int pos = 0;

            for (int j = 1; j < frameSize; j++)
                if (recent[j] < recent[pos]) // Smaller tick = used longer ago
                    pos = j;

            // Evict that frame and load the new page in its place
            frames[pos] = pages[i];
            recent[pos] = i;

            printf("Page %d -> Fault\n", pages[i]);
        }

        displayFrames(frames, frameSize); // Show frame state after this reference
    }

    // Print summary statistics
    printf("\nHits       : %d\n", hits);
    printf("Faults     : %d\n", faults);
    printf("Hit Ratio  : %.2f\n", (float)hits / n);
    printf("Miss Ratio : %.2f\n", (float)faults / n);
}

int main()
{
    int pageSize, frameSize, n;

    // Get simulation parameters from the user
    printf("Enter Page Size (bytes): ");
    scanf("%d", &pageSize);

    printf("Enter Number of Frames: ");
    scanf("%d", &frameSize);

    printf("Enter Number of Page References: ");
    scanf("%d", &n);

    int pages[n]; // The page reference string to simulate

    printf("Enter Page Reference String:\n");
    for (int i = 0; i < n; i++)
        scanf("%d", &pages[i]);

    printf("\nPage Size: %d bytes\n", pageSize);

    // Run both algorithms on the same reference string so their
    // hit/fault performance can be directly compared
    FIFO(pages, n, frameSize);
    LRU(pages, n, frameSize);

    return 0;
}
