#include <stdio.h>

#define MAX 20

// Display current memory frames
void displayFrames(int frames[], int frameSize)
{
    printf("Frames: ");
    for (int i = 0; i < frameSize; i++)
    {
        if (frames[i] == -1)
            printf("- ");
        else
            printf("%d ", frames[i]);
    }
    printf("\n");
}

// FIFO Page Replacement Algorithm
void FIFO(int pages[], int n, int frameSize)
{
    int frames[MAX], front = 0;
    int hits = 0, faults = 0;

    // Initialize empty frames
    for (int i = 0; i < frameSize; i++)
        frames[i] = -1;

    printf("\n========== FIFO ==========\n");

    for (int i = 0; i < n; i++)
    {
        int found = 0;

        // Check if page is already in memory
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
            hits++;
            printf("Page %d -> Hit\n", pages[i]);
        }
        else
        {
            faults++;
            frames[front] = pages[i];
            front = (front + 1) % frameSize;
            printf("Page %d -> Fault\n", pages[i]);
        }

        displayFrames(frames, frameSize);
    }

    printf("\nFIFO Results\n");
    printf("Hits       : %d\n", hits);
    printf("Faults     : %d\n", faults);
    printf("Hit Ratio  : %.2f\n", (float)hits / n);
    printf("Miss Ratio : %.2f\n", (float)faults / n);
}

// LRU Page Replacement Algorithm
void LRU(int pages[], int n, int frameSize)
{
    int frames[MAX], recent[MAX];
    int hits = 0, faults = 0;

    // Initialize empty frames
    for (int i = 0; i < frameSize; i++)
    {
        frames[i] = -1;
        recent[i] = -1;
    }

    printf("\n========== LRU ==========\n");

    for (int i = 0; i < n; i++)
    {
        int found = -1;

        // Check if page is already in memory
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
            hits++;
            recent[found] = i;
            printf("Page %d -> Hit\n", pages[i]);
        }
        else
        {
            faults++;

            int pos = 0;

            // Find least recently used page
            for (int j = 1; j < frameSize; j++)
            {
                if (recent[j] < recent[pos])
                    pos = j;
            }

            frames[pos] = pages[i];
            recent[pos] = i;

            printf("Page %d -> Fault\n", pages[i]);
        }

        displayFrames(frames, frameSize);
    }

    printf("\nLRU Results\n");
    printf("Hits       : %d\n", hits);
    printf("Faults     : %d\n", faults);
    printf("Hit Ratio  : %.2f\n", (float)hits / n);
    printf("Miss Ratio : %.2f\n", (float)faults / n);
}

int main()
{
    int pageSize, logicalAddress;
    int pageNumber, offset;

    printf("Enter Page Size (bytes): ");
    scanf("%d", &pageSize);

    printf("Enter Logical Address: ");
    scanf("%d", &logicalAddress);

    // Calculate page number and offset
    pageNumber = logicalAddress / pageSize;
    offset = logicalAddress % pageSize;

    printf("\n===== Paging System =====\n");
    printf("Page Size       : %d bytes\n", pageSize);
    printf("Logical Address : %d\n", logicalAddress);
    printf("Page Number     : %d\n", pageNumber);
    printf("Offset          : %d\n", offset);

    int frameSize, n;
    int pages[MAX];

    printf("\nEnter Number of Frames: ");
    scanf("%d", &frameSize);

    printf("Enter Number of Page References: ");
    scanf("%d", &n);

    printf("Enter Page Reference String:\n");
    for (int i = 0; i < n; i++)
        scanf("%d", &pages[i]);

    FIFO(pages, n, frameSize);
    LRU(pages, n, frameSize);

    return 0;
}
