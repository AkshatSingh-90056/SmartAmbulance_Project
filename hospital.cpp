#include "hospital.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <stdexcept>

using namespace std;

// ============================================================
//  Hospital Constructor
// ============================================================
Hospital::Hospital(int id, const string& name, int beds, int icu, int nodeId)
    : id(id), name(name), availableBeds(beds), icuBeds(icu), nodeId(nodeId) {}


// ============================================================
//  HospitalManager Constructor - Hardcoded hospital list
// ============================================================
HospitalManager::HospitalManager() {
    hospitals.emplace_back(1, "City General Hospital",   20, 5,  1);
    hospitals.emplace_back(2, "Apollo Medical Center",   10, 8,  3);
    hospitals.emplace_back(3, "Sunrise Trauma Hospital", 5,  12, 5);
    hospitals.emplace_back(4, "Green Valley Clinic",     30, 0,  7);
    hospitals.emplace_back(5, "Metro ICU Specialist",    8,  15, 9);
}


// ============================================================
//  addHospital
// ============================================================
void HospitalManager::addHospital(const Hospital& hospital) {
    hospitals.push_back(hospital);
}


// ============================================================
//  calculateScore  (private)
//  Severity 3 (Critical): Score = (icu*50) + (beds*10) - (dist*5)
//                         if icu == 0 -> Score -= 100000
//  Severity 2 (Medium):   Score = (icu*20) + (beds*20) - (dist*4)
//  Severity 1 (Low):      Score = (beds*15) + (icu*5)  - (dist*6)
// ============================================================
double HospitalManager::calculateScore(const Hospital& h, int dist, int severity) const {
    double score = 0.0;
    switch (severity) {
        case 3:
            score = (h.icuBeds * 50.0) + (h.availableBeds * 10.0) - (dist * 5.0);
            if (h.icuBeds == 0) score -= 100000.0;
            break;
        case 2:
            score = (h.icuBeds * 20.0) + (h.availableBeds * 20.0) - (dist * 4.0);
            break;
        case 1:
            score = (h.availableBeds * 15.0) + (h.icuBeds * 5.0) - (dist * 6.0);
            break;
        default:
            score = (h.icuBeds * 20.0) + (h.availableBeds * 20.0) - (dist * 4.0);
            break;
    }
    return score;
}


// ============================================================
//  findBestHospital
//  Takes severity and distanceMap from Dijkstra (Team 1)
//  Returns index of best hospital, or -1 if none found
// ============================================================
int HospitalManager::findBestHospital(int severity, const vector<int>& distanceMap) const {
    if (hospitals.empty()) {
        cerr << "[ERROR] No hospitals in the system.\n";
        return -1;
    }

    int    bestIndex = -1;
    double bestScore = numeric_limits<double>::lowest();

    for (int i = 0; i < (int)hospitals.size(); i++) {
        const Hospital& h = hospitals[i];

        if (h.nodeId < 0 || h.nodeId >= (int)distanceMap.size()) continue;

        int dist = distanceMap[h.nodeId];
        if (dist == numeric_limits<int>::max() || dist < 0) continue;

        double score = calculateScore(h, dist, severity);

        if (score > bestScore) {
            bestScore = score;
            bestIndex = i;
        }
    }

    return bestIndex;
}


// ============================================================
//  getHospital
// ============================================================
const Hospital& HospitalManager::getHospital(int index) const {
    if (index < 0 || index >= (int)hospitals.size())
        throw out_of_range("Hospital index out of range: " + to_string(index));
    return hospitals[index];
}


// ============================================================
//  count
// ============================================================
int HospitalManager::count() const {
    return (int)hospitals.size();
}
