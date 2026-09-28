#include <stdio.h>
#include <string.h>

#include "graph.h"
#include "console_ui.h"

int graph[MAX_LOCATIONS][MAX_LOCATIONS];
char locationNames[MAX_LOCATIONS][100];
int locationCount = 0;


void initializeGraph(void)
{
    int i;
    int j;

    locationCount = 0;

    for (i = 0; i < MAX_LOCATIONS; i++)
    {
        for (j = 0; j < MAX_LOCATIONS; j++)
        {
            if (i == j)
            {
                graph[i][j] = 0;
            }
            else
            {
                graph[i][j] = INF;
            }
        }
    }
}


void addLocation(int locationId, const char *locationName)
{
    if (locationId < 0 || locationId >= MAX_LOCATIONS)
    {
        printf("\nInvalid location ID!\n");
        return;
    }

    strcpy(locationNames[locationId], locationName);

    if (locationId >= locationCount)
    {
        locationCount = locationId + 1;
    }
}


void addRoad(int source,
             int destination,
             int distance)
{
    if (source < 0 ||
        source >= MAX_LOCATIONS ||
        destination < 0 ||
        destination >= MAX_LOCATIONS)
    {
        printf("\nInvalid location!\n");
        return;
    }

    if (distance <= 0)
    {
        printf("\nInvalid road distance!\n");
        return;
    }

    graph[source][destination] = distance;
    graph[destination][source] = distance;
}


void displayGraph(void)
{
    int i;
    int j;

    uiHeader("DELIVERY GRAPH");

    for (i = 0; i < locationCount; i++)
    {
        printf("\n%d. %s\n",
               i,
               locationNames[i]);

        for (j = 0; j < locationCount; j++)
        {
            if (graph[i][j] != INF &&
                graph[i][j] != 0)
            {
                printf("   -> %s : %d km\n",
                       locationNames[j],
                       graph[i][j]);
            }
        }
    }

    uiDivider();
}


int shortestPath(int source,
                 int destination)
{
    int distance[MAX_LOCATIONS];
    int visited[MAX_LOCATIONS];

    int i;
    int j;
    int current;
    int minimum;

    if (source < 0 ||
        source >= locationCount ||
        destination < 0 ||
        destination >= locationCount)
    {
        return -1;
    }

    for (i = 0; i < locationCount; i++)
    {
        distance[i] = INF;
        visited[i] = 0;
    }

    distance[source] = 0;

    for (i = 0; i < locationCount; i++)
    {
        current = -1;
        minimum = INF;

        for (j = 0; j < locationCount; j++)
        {
            if (!visited[j] &&
                distance[j] < minimum)
            {
                minimum = distance[j];
                current = j;
            }
        }

        if (current == -1)
        {
            break;
        }

        visited[current] = 1;

        for (j = 0; j < locationCount; j++)
        {
            if (graph[current][j] != INF &&
                !visited[j])
            {
                if (distance[current] +
                    graph[current][j] <
                    distance[j])
                {
                    distance[j] =
                        distance[current] +
                        graph[current][j];
                }
            }
        }
    }

    if (distance[destination] == INF)
    {
        return -1;
    }

    return distance[destination];
}


int findShortestPath(int source,
                     int destination,
                     int path[],
                     int *pathLength)
{
    int distance[MAX_LOCATIONS];
    int previous[MAX_LOCATIONS];
    int visited[MAX_LOCATIONS];

    int i;
    int j;
    int current;
    int minimum;

    int reversePath[MAX_LOCATIONS];
    int reverseLength = 0;

    if (source < 0 ||
        source >= locationCount ||
        destination < 0 ||
        destination >= locationCount)
    {
        return -1;
    }

    for (i = 0; i < locationCount; i++)
    {
        distance[i] = INF;
        previous[i] = -1;
        visited[i] = 0;
    }

    distance[source] = 0;

    for (i = 0; i < locationCount; i++)
    {
        current = -1;
        minimum = INF;

        for (j = 0; j < locationCount; j++)
        {
            if (!visited[j] &&
                distance[j] < minimum)
            {
                minimum = distance[j];
                current = j;
            }
        }

        if (current == -1)
        {
            break;
        }

        visited[current] = 1;

        for (j = 0; j < locationCount; j++)
        {
            if (graph[current][j] != INF &&
                !visited[j])
            {
                if (distance[current] +
                    graph[current][j] <
                    distance[j])
                {
                    distance[j] =
                        distance[current] +
                        graph[current][j];

                    previous[j] = current;
                }
            }
        }
    }

    if (distance[destination] == INF)
    {
        *pathLength = 0;
        return -1;
    }

    current = destination;

    while (current != -1)
    {
        reversePath[reverseLength] = current;
        reverseLength++;

        current = previous[current];
    }

    *pathLength = reverseLength;

    for (i = 0; i < reverseLength; i++)
    {
        path[i] =
            reversePath[reverseLength - 1 - i];
    }

    return distance[destination];
}


void displayShortestPath(int source, int destination)
{
    int path[MAX_LOCATIONS];
    int pathLength;
    int distance;
    int i;

    distance = findShortestPath(source, destination, path, &pathLength);
    if (distance < 0)
    {
        printf("\nNo route exists for those locations.\n");
        return;
    }

    printf("\nShortest route: ");
    for (i = 0; i < pathLength; i++)
    {
        printf("%s", locationNames[path[i]]);
        if (i + 1 < pathLength)
        {
            printf(" -> ");
        }
    }
    printf("\nTotal distance: %d km\n", distance);
}