#include "graph.h"
#include "hospital.h"
#include "Ambulance.h"
#include <climits>
#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

enum ConsoleColor {
    COLOR_DEFAULT = 7,
    COLOR_SUCCESS = 10,
    COLOR_WARNING = 14,
    COLOR_DANGER = 12,
    COLOR_INFO = 11,
    COLOR_TITLE = 13
};

void setColor(ConsoleColor color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetColor() {
    setColor(COLOR_DEFAULT);
}

struct EmergencyRequest {
    int requestId;
    int patientNode;
    int severity; // 1=Low, 2=Medium, 3=Critical
};

struct EmergencyCompare {
    bool operator()(const EmergencyRequest& a, const EmergencyRequest& b) const {
        if (a.severity == b.severity) {
            return a.requestId > b.requestId;
        }
        return a.severity < b.severity; // Higher severity first
    }
};

void showMenu() {
    setColor(COLOR_TITLE);
    cout << "\n=====================================================\n";
    cout << "        SMART AMBULANCE PRIORITY DASHBOARD\n";
    cout << "=====================================================\n";
    resetColor();
    cout << "1) Add Ambulance\n";
    cout << "2) Add Emergency Request\n";
    cout << "3) Process Next Request (Highest Priority)\n";
    cout << "4) Process All Pending Requests\n";
    cout << "5) Display Ambulances\n";
    cout << "6) View Pending Requests\n";
    cout << "7) Exit\n";
    cout << "-----------------------------------------------------\n";
    setColor(COLOR_INFO);
    cout << "Choose option: ";
    resetColor();
}

void printPath(const vector<int>& path) {
    if (path.empty()) {
        cout << "No path available";
        return;
    }
    for (int i = 0; i < (int)path.size(); ++i) {
        cout << path[i];
        if (i + 1 < (int)path.size()) {
            cout << " => ";
        }
    }
}

void printPathCard(
    const string& title,
    int startNode,
    int endNode,
    int distance,
    const vector<int>& path
) {
    setColor(COLOR_INFO);
    cout << "\n+---------------------------------------------------+\n";
    cout << "| " << title << '\n';
    cout << "+---------------------------------------------------+\n";
    resetColor();

    if (distance == INT_MAX || path.empty()) {
        setColor(COLOR_WARNING);
        cout << "  Route Status : NOT REACHABLE\n";
        resetColor();
        cout << "  Start Node   : " << startNode << '\n';
        cout << "  End Node     : " << endNode << '\n';
        return;
    }

    int hops = (int)path.size() - 1;
    setColor(COLOR_SUCCESS);
    cout << "  Route Status : ACTIVE\n";
    resetColor();
    cout << "  Start Node   : " << startNode << '\n';
    cout << "  End Node     : " << endNode << '\n';
    cout << "  Total Cost   : " << distance << '\n';
    cout << "  Hops         : " << hops << '\n';
    cout << "  Path         : ";
    printPath(path);
    cout << '\n';
}

void showPendingRequests(
    priority_queue<EmergencyRequest, vector<EmergencyRequest>, EmergencyCompare> requestQueue
) {
    setColor(COLOR_TITLE);
    cout << "\n================= Pending Requests =================\n";
    resetColor();
    if (requestQueue.empty()) {
        setColor(COLOR_WARNING);
        cout << "No pending emergency requests.\n";
        resetColor();
        setColor(COLOR_TITLE);
        cout << "====================================================\n";
        resetColor();
        return;
    }

    int rank = 1;
    while (!requestQueue.empty()) {
        EmergencyRequest req = requestQueue.top();
        requestQueue.pop();
        if (req.severity == 3) {
            setColor(COLOR_DANGER);
        } else if (req.severity == 2) {
            setColor(COLOR_WARNING);
        } else {
            setColor(COLOR_SUCCESS);
        }
        cout << "#" << rank++
             << " | Request ID: " << req.requestId
             << " | Severity: " << req.severity
             << " | Patient Node: " << req.patientNode << '\n';
        resetColor();
    }
    setColor(COLOR_TITLE);
    cout << "====================================================\n";
    resetColor();
}

void processOneRequest(
    const EmergencyRequest& req,
    Graph& city,
    AmbulanceModule& ambulanceModule,
    HospitalManager& hospitalManager
) {
    vector<int> distanceFromPatient = city.dijkstra(req.patientNode);
    int nearestAmbulanceId = ambulanceModule.findNearestAmbulance(distanceFromPatient);

    if (req.severity == 3) {
        setColor(COLOR_DANGER);
    } else if (req.severity == 2) {
        setColor(COLOR_WARNING);
    } else {
        setColor(COLOR_SUCCESS);
    }
    cout << "\nRequest ID: " << req.requestId
         << " | Patient Node: " << req.patientNode
         << " | Severity: " << req.severity << '\n';
    resetColor();

    if (nearestAmbulanceId == -1) {
        setColor(COLOR_DANGER);
        cout << "No available ambulance right now.\n";
        resetColor();
        return;
    }

    int ambulanceNode = ambulanceModule.getAmbulanceLocation(nearestAmbulanceId);
    int ambulanceToPatientDistance = INT_MAX;
    if (ambulanceNode >= 0 && ambulanceNode < (int)distanceFromPatient.size()) {
        ambulanceToPatientDistance = distanceFromPatient[ambulanceNode];
    }

    ambulanceModule.dispatchAmbulance(nearestAmbulanceId, req.patientNode);
    int bestHospitalIndex = hospitalManager.findBestHospital(req.severity, distanceFromPatient);

    setColor(COLOR_INFO);
    cout << "Assigned Ambulance ID: " << nearestAmbulanceId << '\n';
    resetColor();
    if (ambulanceToPatientDistance != INT_MAX) {
        vector<int> ambulanceToPatientPath = city.shortestPath(ambulanceNode, req.patientNode);
        printPathCard(
            "Ambulance -> Patient",
            ambulanceNode,
            req.patientNode,
            ambulanceToPatientDistance,
            ambulanceToPatientPath
        );
    } else {
        printPathCard(
            "Ambulance -> Patient",
            ambulanceNode,
            req.patientNode,
            INT_MAX,
            {}
        );
    }

    if (bestHospitalIndex != -1) {
        const Hospital& h = hospitalManager.getHospital(bestHospitalIndex);
        int patientToHospitalDistance = distanceFromPatient[h.nodeId];
        setColor(COLOR_SUCCESS);
        cout << "Recommended Hospital: " << h.name
             << " (Node " << h.nodeId << ")\n";
        resetColor();
        if (patientToHospitalDistance != INT_MAX) {
            vector<int> patientToHospitalPath = city.shortestPath(req.patientNode, h.nodeId);
            printPathCard(
                "Patient -> Hospital",
                req.patientNode,
                h.nodeId,
                patientToHospitalDistance,
                patientToHospitalPath
            );
        } else {
            printPathCard(
                "Patient -> Hospital",
                req.patientNode,
                h.nodeId,
                INT_MAX,
                {}
            );
        }

        // Demo behavior: Mark ambulance free again at hospital node.
        ambulanceModule.markAmbulanceAvailable(nearestAmbulanceId, h.nodeId);
    } else {
        setColor(COLOR_WARNING);
        cout << "No suitable hospital found.\n";
        resetColor();
        ambulanceModule.markAmbulanceAvailable(nearestAmbulanceId, req.patientNode);
    }
}

int main() {
    Graph city(10);
    city.addEdge(0, 1, 4);
    city.addEdge(0, 2, 6);
    city.addEdge(1, 3, 5);
    city.addEdge(2, 3, 2);
    city.addEdge(2, 4, 3);
    city.addEdge(3, 5, 4);
    city.addEdge(4, 5, 2);
    city.addEdge(5, 6, 5);
    city.addEdge(6, 7, 3);
    city.addEdge(7, 8, 4);
    city.addEdge(8, 9, 2);

    AmbulanceModule ambulanceModule;
    ambulanceModule.addAmbulance(101, 0, true);
    ambulanceModule.addAmbulance(102, 4, true);
    ambulanceModule.addAmbulance(103, 7, true);

    HospitalManager hospitalManager;
    priority_queue<EmergencyRequest, vector<EmergencyRequest>, EmergencyCompare> requestQueue;
    int nextRequestId = 1;

    while (true) {
        int choice;
        showMenu();
        cin >> choice;

        if (choice == 1) {
            int id, locationNode;
            cout << "Enter ambulance ID: ";
            cin >> id;
            cout << "Enter ambulance node (0-9): ";
            cin >> locationNode;
            if (locationNode < 0 || locationNode > 9) {
                setColor(COLOR_WARNING);
                cout << "Invalid node. Must be between 0 and 9.\n";
                resetColor();
                continue;
            }
            ambulanceModule.addAmbulance(id, locationNode, true);
            setColor(COLOR_SUCCESS);
            cout << "Ambulance added and marked available.\n";
            resetColor();
        } else if (choice == 2) {
            EmergencyRequest req{};
            req.requestId = nextRequestId++;
            cout << "Enter patient node (0-9): ";
            cin >> req.patientNode;
            if (req.patientNode < 0 || req.patientNode > 9) {
                setColor(COLOR_WARNING);
                cout << "Invalid node. Must be between 0 and 9.\n";
                resetColor();
                continue;
            }
            cout << "Enter severity (1=Low, 2=Medium, 3=Critical): ";
            cin >> req.severity;
            if (req.severity < 1 || req.severity > 3) {
                setColor(COLOR_WARNING);
                cout << "Invalid severity. Use 1, 2, or 3.\n";
                resetColor();
                continue;
            }
            requestQueue.push(req);
            setColor(COLOR_SUCCESS);
            cout << "Emergency request queued with ID: " << req.requestId << '\n';
            resetColor();
        } else if (choice == 3) {
            if (requestQueue.empty()) {
                setColor(COLOR_WARNING);
                cout << "No pending emergency requests.\n";
                resetColor();
                continue;
            }
            EmergencyRequest req = requestQueue.top();
            requestQueue.pop();
            processOneRequest(req, city, ambulanceModule, hospitalManager);
        } else if (choice == 4) {
            if (requestQueue.empty()) {
                setColor(COLOR_WARNING);
                cout << "No pending emergency requests.\n";
                resetColor();
                continue;
            }
            setColor(COLOR_INFO);
            cout << "\nProcessing all requests by priority...\n";
            resetColor();
            while (!requestQueue.empty()) {
                EmergencyRequest req = requestQueue.top();
                requestQueue.pop();
                processOneRequest(req, city, ambulanceModule, hospitalManager);
            }
        } else if (choice == 5) {
            ambulanceModule.displayAmbulances();
        } else if (choice == 6) {
            showPendingRequests(requestQueue);
        } else if (choice == 7) {
            setColor(COLOR_TITLE);
            cout << "Exiting Smart Ambulance System.\n";
            resetColor();
            break;
        } else {
            setColor(COLOR_WARNING);
            cout << "Invalid option. Please choose between 1 and 7.\n";
            resetColor();
        }
    }

    return 0;
}