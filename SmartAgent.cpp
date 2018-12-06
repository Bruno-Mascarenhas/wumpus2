#include "SmartAgent.h"

void SmartAgent::notify_perceptions
  (bool stench, bool breeze, bool glitter, bool bump, bool scream)
{
  mLast_perceptions[0] = stench;
  mLast_perceptions[1] = breeze;
  mLast_perceptions[2] = glitter;
  mLast_perceptions[3] = bump;
  mLast_perceptions[4] = scream;
}

action SmartAgent::next_action() {

  if (!mLast_perceptions[STENCH] && !mLast_perceptions[BREEZE]) {
    mSeen[{mX+1, mY}] = true;
    mSeen[{mX-1, mY}] = true;
    mSeen[{mX, mY+1}] = true;
    mSeen[{mX, mY-1}] = true;
  }

  if (mLast_perceptions[BUMP]) {
    mLimit[mDir] = mDir&1 ? mY : mX; //if south or north...
    mX -= dirX[mDir];
    mY -= dirY[mDir];
  }

  if (mLast_perceptions[GLITTER]) {
    mHas_gold = true;
    return GRAB; //grab the gold
  }

  if (mLast_perceptions[SCREAM])
    mWumpus_is_alive = false;

  action next_action = bfs();

  return next_action;
}

action SmartAgent::bfs() {
  iii destiny = {mDir, {mX, mY}};
  std::map<iii, iii> link;
  std::set<iii> visited;
  std::queue<iii> q;

  q.push({mDir, {mX, mY}});
  visited.insert({mDir, {mX, mY}});

  while (!q.empty()) {
    iii top = q.front();
    q.pop();

    int dir = top.first;
    int x = top.second.first;
    int y = top.second.second;

    if (!mHas_gold && !mVisited[{x, y}] && destiny == (iii){mDir, {mX, mY}}) {
      //se ainda nao estou com o ouro, vou para o primeiro quadrado visto e nao visitado
      destiny = top;
    }

    //turn right
    if (!visited.count({(dir+1)%4, {x, y}})) {
      link[{(dir+1)%4, {x, y}}] = top;
      visited.insert({(dir+1)%4, {x, y}});
      q.push({(dir+1)%4, {x, y}});
    }

    //turn left
    if (!visited.count({(dir+4-1)%4, {x, y}})) {
      link[{(dir+4-1)%4, {x, y}}] = top;
      visited.insert({(dir+4-1)%4, {x, y}});
      q.push({(dir+4-1)%4, {x, y}});
    }

    //move forward
    int nx = x + dirX[dir], ny = y + dirY[dir];
    if (visited.count({dir, {nx, ny}}) || !mSeen.count({nx, ny})) continue;
    if (!mLimit[dir]) {
      link[{dir, {nx, ny}}] = top;
      visited.insert({dir, {nx, ny}});
      q.push({dir, {nx, ny}});
      continue;
    }

    if ((dir == NORTH && ny < mLimit[NORTH]) || (dir == SOUTH && ny > mLimit[SOUTH]) 
        || (dir == EAST && nx < mLimit[EAST]) || (dir == WEST && nx > mLimit[WEST])) 
    {
      link[{dir, {nx, ny}}] = top;
      visited.insert({dir, {nx, ny}});
      q.push({dir, {nx, ny}});
    }
  }

  if (destiny == (iii){mDir, {mX, mY}})
    return NONE;

  while (link[destiny] != (iii){mDir, {mX, mY}})
    destiny = link[destiny];

  if (destiny.first != mDir) {
    mDir = destiny.first;
    return ((mDir + 1)%4 == destiny.first) ? TURN_RIGHT : TURN_LEFT;
  }

  if (destiny.second.first != mX || destiny.second.second != mY) {
    mX = destiny.second.first;
    mY = destiny.second.second;
    return FORWARD;
  }

  return NONE;
}
