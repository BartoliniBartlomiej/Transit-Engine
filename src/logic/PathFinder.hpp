#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <limits>
#include <algorithm>

// Struktura krawędzi reprezentująca połączenie między stacjami
struct Edge {
    int targetStationId;
    double weight; // Może to być dystans (km) lub czas (minuty)
};

class PathFinder {
private:
    // Reprezentacja grafu: ID stacji -> lista połączeń wychodzących
    std::unordered_map<int, std::vector<Edge>> graph;

public:
    // Dodaje jednokierunkowe połączenie. 
    // Jeśli trasy są dwukierunkowe, wywołaj tę funkcję dwukrotnie dla obu kierunków.
    void addEdge(int source, int target, double weight) {
        graph[source].push_back({target, weight});
    }

    // Znajduje najkrótszą ścieżkę. 
    // Zwraca parę: (łączny_koszt, wektor_ID_stacji_po_kolei)
    std::pair<double, std::vector<int>> findShortestPath(int startStation, int endStation) {
        // Mapy do przechowywania najkrótszych dystansów i poprzednich węzłów (do odtworzenia ścieżki)
        std::unordered_map<int, double> distances;
        std::unordered_map<int, int> previous;
        
        // Kolejka priorytetowa (min-heap): przechowuje pary (dystans, ID stacji)
        using NodePair = std::pair<double, int>;
        std::priority_queue<NodePair, std::vector<NodePair>, std::greater<NodePair>> pq;

        // Inicjalizacja dystansów na "nieskończoność"
        for (const auto& pair : graph) {
            distances[pair.first] = std::numeric_limits<double>::infinity();
        }
        
        // Stacja początkowa ma dystans 0
        distances[startStation] = 0.0;
        pq.push({0.0, startStation});

        while (!pq.empty()) {
            double currentDist = pq.top().first;
            int currentStation = pq.top().second;
            pq.pop();

            // Jeśli dotarliśmy do celu, możemy przerwać poszukiwania (optymalizacja)
            if (currentStation == endStation) {
                break; 
            }

            // Pomijamy przestarzałe wpisy w kolejce priorytetowej
            if (currentDist > distances[currentStation]) {
                continue; 
            }

            // Przeglądamy sąsiadów obecnej stacji
            for (const auto& edge : graph[currentStation]) {
                double newDist = currentDist + edge.weight;
                
                // Jeśli znaleźliśmy krótszą ścieżkę do sąsiada (lub sąsiad nie był jeszcze odwiedzony)
                if (distances.find(edge.targetStationId) == distances.end() || newDist < distances[edge.targetStationId]) {
                    distances[edge.targetStationId] = newDist;
                    previous[edge.targetStationId] = currentStation;
                    pq.push({newDist, edge.targetStationId});
                }
            }
        }

        // Odtwarzanie ścieżki
        std::vector<int> path;
        
        // Jeśli nie znaleziono dystansu do stacji końcowej, to znaczy że nie ma połączenia
        if (distances.find(endStation) == distances.end() || distances[endStation] == std::numeric_limits<double>::infinity()) {
            return {-1.0, path}; // Zwracamy -1.0 jako znacznik błędu
        }

        int current = endStation;
        while (current != startStation) {
            path.push_back(current);
            current = previous[current];
        }
        path.push_back(startStation);
        
        // Ścieżka jest od tyłu (od celu do startu), więc musimy ją odwrócić
        std::reverse(path.begin(), path.end());

        return {distances[endStation], path};
    }
};

#endif // PATHFINDER_HPP