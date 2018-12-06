#ifndef SMARTAGENT
#define SMARTAGENT

#include <map>
#include <set>
#include <queue>

typedef enum action {TURN_RIGHT, TURN_LEFT, FORWARD, SHOOT, GRAB, CLIMB, NONE} action;
typedef enum direction {EAST, SOUTH, WEST, NORTH} direction;
typedef enum perception {STENCH, BREEZE, GLITTER, BUMP, SCREAM} perception;

typedef std::pair <int, int> pii;
typedef std::pair <int, pii> iii;

class SmartAgent {
  private:
    int mX = 0, mY = 0;
    int mDir = EAST;
    int mLimit[4] = {0, 0, 0, 0};
    bool mLast_perceptions[5];
    bool mHas_gold = false;
    bool mWumpus_is_alive = true;
    std::map <pii, bool> mVisited, mSeen;

    int dirX[4] = {1, 0, -1, 0};
    int dirY[4] = {0, 1, 0, -1};

    action bfs();

  public:
    SmartAgent() {
      mVisited[{0, 0}] = true;
      mSeen[{0, 0}] = true;
    }

    void notify_perceptions(bool, bool, bool, bool, bool);
    action next_action();
};

#endif
