#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <string>
#include <vector>

// ============================================================
//  Hospital Structure
// ============================================================
struct Hospital {
    int         id;
    std::string name;
    int         availableBeds;
    int         icuBeds;
    int         nodeId;

    Hospital(int id, const std::string& name, int beds, int icu, int nodeId);
};

// ============================================================
//  HospitalManager Class
// ============================================================
class HospitalManager {
public:
    HospitalManager();

    void addHospital(const Hospital& hospital);

    // distanceMap[nodeId] = shortest distance from patient (Dijkstra output)
    int findBestHospital(int severity, const std::vector<int>& distanceMap) const;

    const Hospital& getHospital(int index) const;

    int count() const;

private:
    std::vector<Hospital> hospitals;

    double calculateScore(const Hospital& h, int dist, int severity) const;
};

#endif // HOSPITAL_H
