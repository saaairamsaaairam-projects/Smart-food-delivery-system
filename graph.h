#ifndef GRAPH_H
#define GRAPH_H

#define MAX_LOCATIONS 20
#define INF 999999

void initializeGraph(void);

void addLocation(int locationId, const char *locationName);

void addRoad(int source,
             int destination,
             int distance);

void displayGraph(void);

int shortestPath(int source,
                 int destination);

/* Finds the shortest route and stores the route in path[] */
int findShortestPath(int source,
                     int destination,
                     int path[],
                     int *pathLength);

void displayShortestPath(int source, int destination);

#endif