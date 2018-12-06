#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string.h>

#include "SmartAgent.h"

const int NEIGHBORS = 4, SQUARE = 80, BORDER = 1;

int wg, hg;
int dirX[] = {1, 0, -1, 0};
int dirY[] = {0, 1, 0, -1};

struct Hunter {
  int y;
  int x;
  int dir = EAST;
  int arrows = 1;
  bool has_gold = false;

  bool **visited;

  sf::Texture texture;
  sf::Sprite sprite;

  Hunter (int xo, int yo, int n, int m) {
    y = yo;
    x = xo;

    visited = new bool*[n];
    for (int i = 0; i < n; i++)
      visited[i] = new bool[m];

    if (!texture.loadFromFile("resources/hunter.png", sf::IntRect(0, 0, SQUARE, SQUARE))) 
      fprintf(stderr, "resources/hunter.png not found\n");

    sprite.setTexture(texture);
    sprite.setOrigin(SQUARE/2 , SQUARE/2);

    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++)
        visited[i][j] = 0;

    visited[y][x] = true;
  }

};

struct Wumpus {
  int y;
  int x;
  bool alive = true;
  bool viewed = false;

  sf::Texture texture, dead;
  sf::Sprite sprite, dead_sprite;

  Wumpus (int xo, int yo, int n, int m) {
    do {
      y = rand()%(n-2) + 1;
      x = rand()%(m-2) + 1;
    } while(y == yo && x == xo);

    if (!texture.loadFromFile("resources/wumpus.png", sf::IntRect(0, 0, SQUARE, SQUARE))) 
      fprintf(stderr, "resources/wumpus.png not found\n");

    sprite.setTexture(texture);
    sprite.setPosition(sf::Vector2f(x * SQUARE + BORDER + wg + SQUARE/2,
          y * SQUARE + BORDER + hg + SQUARE/2));
    sprite.setOrigin(SQUARE/2 , SQUARE/2);

    if (!dead.loadFromFile("resources/dead.png", sf::IntRect(0, 0, SQUARE, SQUARE))) 
      fprintf(stderr, "resources/dead.png not found\n");

    dead_sprite.setTexture(dead);
    dead_sprite.setPosition(sf::Vector2f(x * SQUARE + BORDER + wg, y * SQUARE + BORDER + hg));
  }

};

struct Treasure {
  int y;
  int x;
  bool has_gold = true;
  bool viewed = false;

  sf::Texture texture, gold;
  sf::Sprite sprite, gold_sprite;

  Treasure (int xo, int yo, int n, int m) {
    do {
      y = rand()%(n-2) + 1;
      x = rand()%(m-2) + 1;
    } while(y == yo && x == xo);

    if (!texture.loadFromFile("resources/treasure.png", sf::IntRect(0, 0, SQUARE, SQUARE))) 
      fprintf(stderr, "resources/treasure.png not found\n");

    if (!gold.loadFromFile("resources/gold.png", sf::IntRect(0, 0, SQUARE, SQUARE))) 
      fprintf(stderr, "resources/gold.png not found\n");

    sprite.setTexture(texture);
    sprite.setPosition(sf::Vector2f(x * SQUARE + BORDER + wg, y * SQUARE + BORDER + hg));

    gold_sprite.setTexture(gold);
    gold_sprite.setPosition(sf::Vector2f(x * SQUARE + BORDER + wg, y * SQUARE + BORDER + hg));
  }
};

int main () {
  srand(time(NULL));
  char play_again;
  printf("Welcome to the Wumpus World!\n\n\
Created by:\nIgor Silva\n\n");

  do {
    int n = 5 + rand()%5;
    int m = 7 + rand()%7;
    int yo = rand()%(n-2) + 1;
    int xo = rand()%(m-2) + 1;

    char op;
    printf("Human player/Intelligent agent(h/I)");
    op = getchar();
    while (op != '\n' && getchar() != '\n');

    int fps = (op == 'h') ? 10 : 3;

    wg = sf::VideoMode::getDesktopMode().width/2 - (m*SQUARE + BORDER)/2;
    hg = sf::VideoMode::getDesktopMode().height/2 - (n*SQUARE + BORDER)/2;

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Wumpus world",
        sf::Style::Fullscreen);

    window.setFramerateLimit(fps);

    sf::Music music;
    if (!music.openFromFile("resources/Exploring_a_cave.ogg"))
      fprintf(stderr, "resources/Exploring_a_cave.ogg not found\n");
    music.setLoop(true);
    music.play();

    sf::Font default_font;
    if (!default_font.loadFromFile("resources/yudit.ttf"))
      fprintf(stderr, "resources/yudit.ttf not found\n");

    sf::Text bumpText;
    bumpText.setFont(default_font);
    bumpText.setCharacterSize(20); //this should be a const
    bumpText.setColor(sf::Color::Red);
    bumpText.setString("Bump");

    sf::Text stenchText;
    stenchText.setFont(default_font);
    stenchText.setCharacterSize(20); //this should be a const
    stenchText.setColor(sf::Color::Red);
    stenchText.setString("Stench");

    sf::Text breezeText;
    breezeText.setFont(default_font);
    breezeText.setCharacterSize(20); //this should be a const
    breezeText.setColor(sf::Color::Red);
    breezeText.setString("Breeze");

    sf::Text gameOverText;
    gameOverText.setFont(default_font);
    gameOverText.setCharacterSize(80); //this should be a const
    gameOverText.setColor(sf::Color::Red);
    gameOverText.setPosition(sf::Vector2f(BORDER + wg, BORDER + hg));

    sf::Text screamText;
    screamText.setFont(default_font);
    screamText.setCharacterSize(80); //this should be a const
    screamText.setColor(sf::Color::Red);
    screamText.setPosition(sf::Vector2f(BORDER + wg, BORDER + hg));
    screamText.setString("The wumpus died");

    sf::SoundBuffer bumpSoundBuffer;
    if (!bumpSoundBuffer.loadFromFile("resources/bump.wav"))
      fprintf(stderr, "resources/bump.wav not found\n");
    sf::Sound bumpSound(bumpSoundBuffer);
    bumpSound.setVolume(70);

    sf::SoundBuffer stenchSoundBuffer;
    if (!stenchSoundBuffer.loadFromFile("resources/stench.ogg"))
      fprintf(stderr, "resources/stench.ogg not found\n");
    sf::Sound stenchSound(stenchSoundBuffer);

    sf::SoundBuffer lossSoundBuffer;
    if (!lossSoundBuffer.loadFromFile("resources/loss.ogg"))
      fprintf(stderr, "resources/loss.ogg not found\n");
    sf::Sound lossSound(lossSoundBuffer);

    sf::SoundBuffer climbSoundBuffer;
    if (!climbSoundBuffer.loadFromFile("resources/climb.wav"))
      fprintf(stderr, "resources/climb.ogg not found\n");
    sf::Sound climbSound(climbSoundBuffer);

    sf::SoundBuffer screamSoundBuffer;
    if (!screamSoundBuffer.loadFromFile("resources/scream.wav"))
      fprintf(stderr, "resources/scream.ogg not found\n");
    sf::Sound screamSound(screamSoundBuffer);

    sf::SoundBuffer failSoundBuffer;
    if (!failSoundBuffer.loadFromFile("resources/fail.wav"))
      fprintf(stderr, "resources/fail.ogg not found\n");
    sf::Sound failSound(failSoundBuffer);

    sf::Texture hole;
    if (!hole.loadFromFile("resources/hole.png")) 
      fprintf(stderr, "resources/hole.png not found\n");

    sf::Texture gold;
    if (!gold.loadFromFile("resources/gold.png")) 
      fprintf(stderr, "resources/gold.png not found\n");

    sf::Sprite gold_sprite;
    gold_sprite.setTexture(gold);
    gold_sprite.setPosition(sf::Vector2f(0, 10));

    sf::Texture arrow;
    if (!arrow.loadFromFile("resources/arrow.png")) 
      fprintf(stderr, "resources/arrow.png not found\n");

    sf::Sprite arrow_sprite;
    arrow_sprite.setTexture(arrow);
    arrow_sprite.setPosition(sf::Vector2f(0, 10));

    sf::Texture cave;
    if (!cave.loadFromFile("resources/cave.png", sf::IntRect(0, 0, SQUARE, SQUARE))) 
      fprintf(stderr, "resources/cave.png not found\n");

    sf::Texture wall;
    if (!wall.loadFromFile("resources/wall.png", sf::IntRect(0, 0, SQUARE, SQUARE))) 
      fprintf(stderr, "resources/wall.png not found\n");

    sf::Texture ladder;
    if (!ladder.loadFromFile("resources/ladder.png", sf::IntRect(0, 0, SQUARE, SQUARE))) 
      fprintf(stderr, "resources/ladder.png not found\n");

    sf::Sprite ladder_sprite;
    ladder_sprite.setTexture(ladder);
    ladder_sprite.setPosition(sf::Vector2f(xo * SQUARE + BORDER + wg, yo * SQUARE + BORDER + hg));

    sf::Texture controls;
    if (!controls.loadFromFile("resources/controls.png")) 
      fprintf(stderr, "resources/controls.png not found\n");

    sf::Sprite controls_sprite;
    controls_sprite.setColor(sf::Color(160, 160, 160));
    controls_sprite.setTexture(controls);
    controls_sprite.setPosition(sf::Vector2f(sf::VideoMode::getDesktopMode().width - 180,
          sf::VideoMode::getDesktopMode().height - 330));

    sf::RectangleShape grid[n][m];
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        grid[i][j].setSize({SQUARE - BORDER, SQUARE - BORDER});
        grid[i][j].setOutlineColor(sf::Color::Black);
        grid[i][j].setOutlineThickness(BORDER);
        grid[i][j].setPosition(j*SQUARE + BORDER + wg, i*SQUARE + BORDER + hg);
        grid[i][j].setTexture(i && j && i<n-1 && j<m-1 ? &cave : &wall, true);
      }
    }

    sf::Event event;
    Hunter hunter(xo, yo, n, m);
    action next_action;
    Wumpus wumpus(xo, yo, n, m);
    Treasure treasure(xo, yo, n, m);
    bool gameover = false;
    int points = 0;

    SmartAgent smartagent;

    bool pit[n][m];
    memset(pit, 0, sizeof(pit));
    for (int i = 1; i < n-1; i++)
      for (int j = 1; j < m-1; j++)
        if (rand()%100 < 20 && (i != hunter.y || j != hunter.x) 
            && (i != treasure.y || j != treasure.x)) 
          pit[i][j] = true;

    bool stench = abs(hunter.x - wumpus.x) + abs(hunter.y - wumpus.y) <= 1;
    bool breeze = pit[hunter.y+1][hunter.x] || pit[hunter.y-1][hunter.x]
      || pit[hunter.y][hunter.x+1] || pit[hunter.y][hunter.x-1];

    smartagent.notify_perceptions(stench, breeze, false, false, false);

    while (window.isOpen()) { //game loop

      next_action = NONE;
      while (window.pollEvent(event)) { //process input
        if (event.type == sf::Event::Closed)
          window.close();

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)
          window.close();

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Right)
          next_action = TURN_RIGHT;

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Left)
          next_action = TURN_LEFT;

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Up)
          next_action = FORWARD;

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Space)
          next_action = SHOOT;

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::G)
          next_action = GRAB;

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::C)
          next_action = CLIMB;
      }

      if (gameover) {
        window.display();
        continue;
      }

      if (op != 'h' && op != 'H')
        next_action = smartagent.next_action();

      //update game
      bool bump = false;
      bool kill = false;
      bool hitWall = false;
      if (next_action != NONE)
        points--;

      switch (next_action) {
        case TURN_RIGHT:
          hunter.dir = (hunter.dir + 1) % NEIGHBORS;
          break;

        case TURN_LEFT:
          hunter.dir = (hunter.dir + NEIGHBORS - 1) % NEIGHBORS;
          break;

        case FORWARD:
          hunter.visited[hunter.y + dirY[hunter.dir]][hunter.x + dirX[hunter.dir]] = true;
          static int _xo, _yo;
          _xo = hunter.x;
          _yo = hunter.y;
          hunter.x = std::min(std::max(1, hunter.x + dirX[hunter.dir]), m-2);
          hunter.y = std::min(std::max(1, hunter.y + dirY[hunter.dir]), n-2);
          if (hunter.x == _xo && hunter.y == _yo)
            bump = true;
          break;

        case SHOOT:
          if (hunter.arrows) {
            hunter.arrows--;
            points -= 10;

            static int diff_x, diff_y;
            diff_x = wumpus.x > hunter.x ? 1 : (wumpus.x < hunter.x ? -1 : 0);
            diff_y = wumpus.y > hunter.y ? 1 : (wumpus.y < hunter.y ? -1 : 0);

            if (dirY[hunter.dir] == diff_y && dirX[hunter.dir] == diff_x) {
              wumpus.alive = false;
              kill = true;
            } else {
              hitWall = true;
            }
          }
          break;

        case GRAB:
          if (abs(hunter.x - treasure.x) + abs(hunter.y - treasure.y) == 0 && treasure.has_gold) {
            treasure.has_gold = false;
            hunter.has_gold = true;
          }
          break;

        case CLIMB:
          if (hunter.x == xo && hunter.y == yo) {
            if (hunter.has_gold)
              points += 1000;

            gameover = true;
          }
          break;
      }

      hunter.sprite.setRotation(hunter.dir * 90);
      hunter.sprite.setPosition(sf::Vector2f(hunter.x * SQUARE + BORDER + SQUARE/2 + wg,
            hunter.y * SQUARE + BORDER + SQUARE/2 + hg));

      wumpus.sprite.setRotation((hunter.dir+2)%NEIGHBORS * 90);

      bumpText.setPosition(sf::Vector2f(hunter.x * SQUARE + BORDER + wg,
            hunter.y * SQUARE + BORDER + hg));

      stenchText.setPosition(sf::Vector2f(hunter.x * SQUARE + BORDER + wg,
            hunter.y * SQUARE + BORDER + hg + 20));

      breezeText.setPosition(sf::Vector2f(hunter.x * SQUARE + BORDER + wg,
            hunter.y * SQUARE + BORDER + hg + 40));

      stench = abs(hunter.x - wumpus.x) + abs(hunter.y - wumpus.y) <= 1;

      bool glitter = abs(hunter.x - treasure.x) + abs(hunter.y - treasure.y) == 0
        && treasure.has_gold;

      breeze = pit[hunter.y+1][hunter.x] || pit[hunter.y-1][hunter.x]
        || pit[hunter.y][hunter.x+1] || pit[hunter.y][hunter.x-1];

      bool eaten = false;
      bool fall = false;

      if (glitter)
        treasure.viewed = true;

      smartagent.notify_perceptions(stench, breeze, glitter, bump, kill);

      if (wumpus.x == hunter.x && wumpus.y == hunter.y) {
        wumpus.viewed = true;
        if (wumpus.alive) {
          gameover = true;
          eaten = true;
        }
      }

      if (pit[hunter.y][hunter.x]) {
        gameover = true;
        fall = true;
      }

      //render
      window.clear(sf::Color::Black);

      for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
          sf::Color gray = sf::Color(120, 120, 120);
          if (i == hunter.y && j == hunter.x)
            gray = sf::Color(200, 200, 200);

          grid[i][j].setFillColor(hunter.visited[i][j] ? gray : sf::Color::Black);
          window.draw(grid[i][j]);

          if (!pit[i][j]) continue;

          sf::Sprite sprite;
          sprite.setTexture(hole);
          sprite.setPosition(sf::Vector2f(j * SQUARE + BORDER + wg, i * SQUARE + BORDER + hg));
          window.draw(sprite);
        }
      }

      window.draw(controls_sprite);

      if (wumpus.viewed && !wumpus.alive)
        window.draw(wumpus.dead_sprite);

      if (hunter.arrows)
        window.draw(arrow_sprite);

      if (hunter.has_gold)
        window.draw(gold_sprite);

      if (kill) {
        screamSound.play();
        window.draw(screamText);
      } else if (hitWall)
        failSound.play();

      if (treasure.viewed) {
        window.draw(treasure.sprite);
        if (treasure.has_gold)
          window.draw(treasure.gold_sprite);
      }

      window.draw(ladder_sprite);
      if (gameover) {
        music.stop();
        if (eaten) {
          window.draw(wumpus.sprite);
          points = points - 1000;
          lossSound.play();
        } else if (fall) {
          points = points - 1000;
          lossSound.play();
        }else {
          climbSound.play();
        }

        char str[20];
        sprintf(str,"GAME OVER\n%d points", points);
        gameOverText.setString(str);
        window.draw(gameOverText);
      } else {
        window.draw(hunter.sprite);
      }

      if (bump) {
        window.draw(bumpText);
        bumpSound.play();
      }

      if (breeze) {
        window.draw(breezeText);
      }

      if (stench) {
        window.draw(stenchText);
        if (wumpus.alive && stenchSound.getStatus() != sf::SoundSource::Status::Playing)
          stenchSound.play();
      }
      if (!stench || !wumpus.alive)
        stenchSound.stop();

      window.display();

      if (kill)
        sf::sleep(sf::milliseconds(500));
    }

    printf("Do you want to play again?(Yes,no)");
    play_again = getchar();
    while (play_again != '\n' && getchar() != '\n');

  } while (play_again != 'n' && play_again != 'N');

  return 0;
}
