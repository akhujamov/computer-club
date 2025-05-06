#include <iostream>
#include <string>
#include <map>
#include <deque>
#include <vector>
#include <algorithm>

struct TimeStruct {
    int hours;
    int minutes;
};

std::string name[10014];
std::string WhoSeat[10014];
std::map<std::string, int> check;
std::map<std::string, int> manager;
int tables, cost, type[10014], n, table[10014], profit[10014], TimeWaste[10014], us[10014], used;
TimeStruct TimeOpen, TimeClose;
TimeStruct Time[10014];

TimeStruct convert(std::string TTime) {
    int colon_pos = TTime.find(':');
    TimeStruct result;
    result.hours = std::stoi(TTime.substr(0, colon_pos));
    result.minutes = std::stoi(TTime.substr(colon_pos + 1));
    return result;
}

std::string convertback(TimeStruct TTime) {
    std::string ans = "";
    if(TTime.hours < 10) {
        ans += "0";
    }
    ans += std::to_string(TTime.hours);
    ans += ":";
    if(TTime.minutes < 10) {
        ans += "0";
    }
    ans += std::to_string(TTime.minutes);
    return ans;
}
void update_profit(int table_id, int start, int end) {
    int duration = end - start;
    profit[table_id] += ((duration + 59) / 60) * cost;
    TimeWaste[table_id] += duration;
    us[table_id] = 0;
    WhoSeat[table_id] = "noOne";
}

int getTotalMinutes(TimeStruct ts) {
    return ts.hours * 60 + ts.minutes;
}

int main() {
    freopen("test_file.txt", "r", stdin);
    freopen("result_file.txt", "w", stdout);

    std::deque<int> WaitList;
    std::string TimeOpenS, TimeCloseS, TimeS;
    std::cin >> tables;
    std::cin >> TimeOpenS >> TimeCloseS;
    TimeOpen = convert(TimeOpenS);
    TimeClose = convert(TimeCloseS);

    if(TimeOpen.hours > 24 || TimeOpen.minutes > 60 ||
       TimeClose.hours > 24 || TimeClose.minutes > 60 ||
       getTotalMinutes(TimeOpen) > getTotalMinutes(TimeClose) ||
        TimeOpenS.size() != 5 || TimeCloseS.size() != 5) {
        std::cout << "Syntax Error" << " " << TimeOpenS << " " << TimeCloseS;
        return 0;
    }

    std::cin >> cost;
    while(std::cin >> TimeS) {
        Time[n] = convert(TimeS);
        std::cin >> type[n] >> name[n];
        if(type[n] == 2) {
            std::cin >> table[n];
            if(table[n] > tables) {
                std::cout << "Syntax Error" << " " << table[n];
            }
        }
        if(Time[n].hours > 24 || Time[n].minutes > 60 || TimeS.size() != 5){
            std::cout << "Syntax Error" << " " << TimeS << " " << type[n] << " " << name[n];
            if(type[n] == 2){
                std::cout << " " << table[n];
            }
            return 0;
        }
        if(type[n] > 4) {
            std::cout << "Syntax Error" << " " << type[n];
            return 0;
        }
        if(n != 0) {
            if(getTotalMinutes(Time[n - 1]) > getTotalMinutes(Time[n])) {
                std::cout << "Syntax Error" << " " << TimeS;
                return 0;
            }
        }
        n++;
    }
    std::cout << TimeOpenS << std::endl;
    for(int i = 0; i < n; i++) {
        std::string cTime = convertback(Time[i]);
        std::cout << cTime << " " << type[i] << " " << name[i];
        if(type[i] == 2)
            std::cout << " " << table[i];
        std::cout << std::endl;

        if(type[i] == 1) {
            if(getTotalMinutes(Time[i]) > getTotalMinutes(TimeClose)) {
                std::cout << cTime << " 13 CloseGoHome" << std::endl;
            }
            else if(getTotalMinutes(Time[i]) < getTotalMinutes(TimeOpen)) {
                std::cout << cTime << " 13 NotOpenYet" << std::endl;
            }
            else if(manager[name[i]]) {
                std::cout << cTime << " 13 YouShallNotPass" << std::endl;
            }
            else
                manager[name[i]] = -1;
        }

        if(type[i] == 2) {
            if(!manager[name[i]]) {
                std::cout << cTime << " 13 ClientUnknown" << std::endl;
            }
            else if(us[table[i]]) {
                std::cout << cTime << " 13 PlaceIsBusy" << std::endl;
            }
            else {
                if(manager[name[i]] > 0) {
                    update_profit(check[name[i]], manager[name[i]], getTotalMinutes(Time[i]));
                    check[name[i]] = 0;
                    used--;
                }
                used++;
                us[table[i]] = 1;
                check[name[i]] = table[i];
                WhoSeat[check[name[i]]] = name[i];
                manager[name[i]] = getTotalMinutes(Time[i]);
            }
        }

        if(type[i] == 3) {
            if(!manager[name[i]]) {
                std::cout << cTime << " 13 ClientUnknown" << std::endl;
            }
            else if(used < tables) {
                std::cout << cTime << " 13 ICanWaitNoLonger!" << std::endl;
            }
            else if(WaitList.size() > tables) {
                std::cout << cTime << " 11 " << name[i] << std::endl;
            }
            else {
                WaitList.push_back(i);
            }
        }

        if(type[i] == 4) {
            if(!manager[name[i]]) {
                std::cout << cTime << " 13 ClientUnknown" << std::endl;
            }
            else {
                update_profit(check[name[i]], manager[name[i]], getTotalMinutes(Time[i]));
                if(WaitList.size()) {
                    us[check[name[i]]] = 1;
                    WhoSeat[check[name[i]]] = name[i];
                    int client = WaitList.front();
                    WaitList.pop_front();
                    check[name[client]] = check[name[i]];
                    manager[name[client]] = getTotalMinutes(Time[i]);
                    std::cout << cTime << " 12 " << name[client] << " " << check[name[i]] << std::endl;
                }
                check[name[i]] = 0;
            }
        }
    }

    std::vector<std::pair<std::string, int>> remaining;
    for(int i = 1; i <= tables; i++) {
        if(us[i])
            remaining.push_back({WhoSeat[i], i});
    }
    std::sort(remaining.begin(), remaining.end(),
        [](const auto& a, const auto& b) {
            return std::lexicographical_compare(
                a.first.begin(), a.first.end(),
                b.first.begin(), b.first.end(),
                [](char c1, char c2) {
                    return std::tolower(c1) < std::tolower(c2);
                }
            );
        }
    );

    for(int i = 0; i < remaining.size(); i++) {
        std::cout << TimeCloseS << " 11 " << remaining[i].first << std::endl;
        profit[remaining[i].second] += ((getTotalMinutes(TimeClose) - manager[remaining[i].first] + 59) / 60) * cost;
        TimeWaste[remaining[i].second] += getTotalMinutes(TimeClose) - manager[remaining[i].first];
    }

    std::cout << TimeCloseS << std::endl;
    for(int i = 1; i <= tables; i++) {
        std::string TimeWasteS = convertback({TimeWaste[i] / 60, TimeWaste[i] % 60});
        std::cout << i << " " << profit[i] << " " << TimeWasteS << std::endl;
    }
}
