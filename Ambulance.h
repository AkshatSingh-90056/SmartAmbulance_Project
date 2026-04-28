#ifndef AMBULANCE_H
#define AMBULANCE_H

#include <string>
#include <vector>

struct Ambulance {
    int id;
    int locationNode; // Node ID for Dijkstra
    bool isAvailable;
};

class AmbulanceModule {
private:
    std::vector<Ambulance> ambulances;

public:
    void addAmbulance(int id, int locationNode, bool available);
    
    // Finds the nearest available ambulance using distances from Dijkstra
    int findNearestAmbulance(const std::vector<int>& distances);
    int getAmbulanceLocation(int id) const;
    bool dispatchAmbulance(int id, int patientNode);
    bool markAmbulanceAvailable(int id, int locationNode);
    
    void displayAmbulances();
};

#endif