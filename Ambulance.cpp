#include "Ambulance.h"
#include <iostream>
#include <climits>

void AmbulanceModule::addAmbulance(int id, int locationNode, bool available) {
    ambulances.push_back({id, locationNode, available});
}

int AmbulanceModule::findNearestAmbulance(const std::vector<int>& distances) {
    int nearestId = -1;
    int minDistance = INT_MAX;

    for (const auto& amb : ambulances) {
        if (amb.isAvailable) {
            int dist = distances[amb.locationNode];
            if (dist < minDistance) {
                minDistance = dist;
                nearestId = amb.id;
            }
        }
    }
    return nearestId; 
}

int AmbulanceModule::getAmbulanceLocation(int id) const {
    for (const auto& amb : ambulances) {
        if (amb.id == id) {
            return amb.locationNode;
        }
    }
    return -1;
}

bool AmbulanceModule::dispatchAmbulance(int id, int patientNode) {
    for (auto& amb : ambulances) {
        if (amb.id == id && amb.isAvailable) {
            amb.isAvailable = false;
            amb.locationNode = patientNode;
            return true;
        }
    }
    return false;
}

bool AmbulanceModule::markAmbulanceAvailable(int id, int locationNode) {
    for (auto& amb : ambulances) {
        if (amb.id == id) {
            amb.isAvailable = true;
            amb.locationNode = locationNode;
            return true;
        }
    }
    return false;
}

void AmbulanceModule::displayAmbulances() {
    std::cout << "\n--- Ambulance List ---\n";
    for (const auto& amb : ambulances) {
        std::cout << "ID: " << amb.id 
                  << " | Location: Node " << amb.locationNode 
                  << " | Status: " << (amb.isAvailable ? "Available" : "Busy") 
                  << std::endl;
    }
}