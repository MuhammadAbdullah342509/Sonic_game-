#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
using namespace sf;
using namespace std;
int screen_x = 1200;
int screen_y = 900;
float getGravityModifier(string name)
{
    if (name == "Level 3")
        return 0.9f;
    return 1.0f;
}
float getAccelerationModifier(string name)
{
    if (name == "Level 2")
        return 0.5f;
    if (name == "Level 3")
        return 0.7f;
    return 0.2f;
}
void draw_player(RenderWindow &window, Sprite &LstillSprite, float player_x, float player_y);
void display_level(RenderWindow &window, const int height, const int width, char **lvl, Sprite &wallSprite1, const int cell_size, float x);
class Obstacle;
class Enemy;
class BeeBot;
class Level;
class Game;
class GameOverMenu;
class player
{
    static int health;

protected:
    virtual bool shouldSkipYClamp() const { return false; }

protected:
    Texture texture;
    Sprite sprite;
    float x, y;
    float velocityX, velocityY;
    float gravity;
    float terminalVelocity;
    float acceleration;
    float maxSpeed;
    float jump_strength;
    bool onGround;
    int rawWidth, rawHeight;
    float width, height;
    float hitBoxX, hitBoxY;
    float flightTime = 0;
    bool invincible = false;

public:
    player() {}
    player(const string &texPath,
           float scale_x, float scale_y,
           float accel = 0.2f,
           float topSpeed = 15.f,
           float jumpStr = 22.f)
        : x(20), y(20),
          velocityX(0), velocityY(0),
          gravity(1.f),
          terminalVelocity(20.f),
          acceleration(accel),
          maxSpeed(topSpeed),
          jump_strength(jumpStr),
          onGround(false),
          rawWidth(24), rawHeight(35)
    {
        texture.loadFromFile(texPath);
        sprite.setTexture(texture);
        sprite.setScale(scale_x, scale_y);
        width = rawWidth * scale_x;
        height = rawHeight * scale_y;
        hitBoxX = 8 * scale_x;
        hitBoxY = 5 * scale_y;

        sprite.setPosition(x, y);
    }

    virtual ~player() {}
    static void decreaseHP()
    {
        if (health < 0)
        {
            health = 0;
        }
        else
        {
            health = health - 1;
        }
    }

    virtual void controller()
    {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            velocityX -= acceleration;
            if (velocityX < -maxSpeed)
                velocityX = -maxSpeed;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            velocityX += acceleration;
            if (velocityX > maxSpeed)
                velocityX = maxSpeed;
        }
        else
        {
            if (velocityX > 0)
            {
                velocityX -= acceleration;
                if (velocityX < 0)
                    velocityX = 0;
            }
            else if (velocityX < 0)
            {
                velocityX += acceleration;
                if (velocityX > 0)
                    velocityX = 0;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && onGround)
        {
            velocityY = -jump_strength;
            onGround = false;
        }
    }
    virtual void player_gravity(char **lvl, int cell_size)
    {
        float offset_y = y + velocityY;
        char bottom_left_down = lvl[(int)(offset_y + hitBoxY + height) / cell_size][(int)(x + hitBoxX) / cell_size];
        char bottom_right_down = lvl[(int)(offset_y + hitBoxY + height) / cell_size][(int)(x + hitBoxX + width) / cell_size];
        char bottom_mid_down = lvl[(int)(offset_y + hitBoxY + height) / cell_size][(int)(x + hitBoxX + width / 2) / cell_size];

        if (bottom_left_down == 'w' || bottom_mid_down == 'w' || bottom_right_down == 'w')
        {
            onGround = true;
            velocityY = 0;
        }
        else
        {
            y = offset_y;
            onGround = false;
            velocityY += gravity;
            if (velocityY >= terminalVelocity)
                velocityY = terminalVelocity;
        }
    }

    virtual void update()
    {
        x += velocityX;
        y += velocityY;
        sprite.setPosition(x, y);
    }

    virtual void draw(RenderWindow &w, float scrollX)
    {
        sprite.setPosition(x - scrollX, y);
        w.draw(sprite);
    }
    virtual bool isKnuckles() const { return false; }
    bool isSonic() const { return false; }
    bool isTails() const { return false; }
    virtual float getX() const { return x; }
    virtual float getY() const { return y; }
    virtual float getWidth() const { return width; }
    virtual float getHeight() const { return height; }
    virtual float getVelocityX() const { return velocityX; }
    virtual float getVelocityY() const { return velocityY; }
    virtual bool getOnGround() const { return onGround; }
    virtual float getHitBoxX() const { return hitBoxX; }
    virtual float getHitBoxY() const { return hitBoxY; }
    virtual void setPosition(float newX, float newY)
    {
        x = newX;
        y = newY;
        sprite.setPosition(x, y);
    }
    virtual void setX(float newX)
    {
        x = newX;
        sprite.setPosition(x, y);
    }
    virtual void setY(float newY)
    {
        y = newY;
        sprite.setPosition(x, y);
    }
    virtual void setVelocityX(float vx) { velocityX = vx; }
    virtual void setVelocityY(float vy) { velocityY = vy; }
    virtual void setOnGround(bool ground) { onGround = ground; }
    static void increaseHP(int val)
    {
        health += val;
    }

    virtual void setMaxSpeed(float speed)
    {
        maxSpeed = speed;
    }
    virtual float getMaxSpeed() const
    {
        return maxSpeed;
    }
    virtual void increaseFlightTime(float seconds)
    {
        flightTime += seconds;
    }
    virtual void resetFlightTime()
    {
        flightTime = defaultFlightTime();
    }
    virtual void setInvincible(bool inv)
    {
        invincible = inv;
    }
    virtual bool isInvincible() const
    {
        return invincible;
    }
    virtual float defaultFlightTime() const
    {
        return 5.0f;
    }
    virtual void setGravityBasedOnLevel(const string &levelName)
    {
        acceleration = getGravityModifier(levelName);
    }
    virtual void setAccelerationBasedOnLevel(const string &levelName)
    {
        gravity = getGravityModifier(levelName);
    }
    void clampPosition(int mapWidth, int mapHeight)
    {
        if (x < 0)
            x = 0;
        if (x + width > mapWidth)
            x = mapWidth - width;
        if (y < 0)
            y = 0;
        if (!shouldSkipYClamp() && y + height > mapHeight)
            y = mapHeight - height;
        sprite.setPosition(x, y);
    }
};
int player::health = 3;

class Obstacle
{
protected:
    float x, y, width, height;

public:
    virtual ~Obstacle() {}
    virtual void check(player &p) = 0;
    virtual void draw(sf::RenderWindow &window, float x) {}
    virtual bool isWall() const { return false; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
};
class Wall : public Obstacle
{
private:
    Sprite sprite;

public:
    Wall(float x_, float y_, float w_, float h_, Texture &texture)
    {
        x = x_;
        y = y_;
        width = w_;
        height = h_;
        sprite.setTexture(texture);
        sprite.setScale(this->width / texture.getSize().x, this->height / texture.getSize().y);
        sprite.setPosition(x, y);
    }
    bool isWall() const override { return true; }
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    void check(player &p) override
    {
        const float EPSILON = 0.0001f;
        float px = p.getX();
        float py = p.getY();
        float pw = p.getWidth();
        float ph = p.getHeight();
        float pvx = p.getVelocityX();
        float pvy = p.getVelocityY();

        float next_px = px + pvx;
        float next_py = py + pvy;

        float playerLeft = next_px;
        float playerRight = next_px + pw;
        float playerTop = next_py;
        float playerBottom = next_py + ph;

        float wallLeft = x;
        float wallRight = x + width;
        float wallTop = y;
        float wallBottom = y + height;

        if (playerRight > wallLeft && playerLeft < wallRight &&
            playerBottom > wallTop && playerTop < wallBottom)
        {

            float penLeft = playerRight - wallLeft;
            float penRight = wallRight - playerLeft;
            float penTop = playerBottom - wallTop;
            float penBottom = wallBottom - playerTop;

            float minPen = std::min({penLeft, penRight, penTop, penBottom});

            if (minPen == penLeft)
            {
                p.setX(wallLeft - pw - EPSILON);
                p.setVelocityX(0);
            }
            else if (minPen == penRight)
            {

                if (pvx < 0)
                {
                    p.setX(wallRight + EPSILON);
                    p.setVelocityX(0);
                }
            }
            else if (minPen == penTop)
            {
                p.setY(wallTop - ph - EPSILON);
                p.setVelocityY(0);
                p.setOnGround(true);
            }
            else if (minPen == penBottom)
            {
                p.setY(wallBottom);
                p.setVelocityY(0);
            }
        }

        if (pvx < 0 &&
            px + pw > x && px < x + width &&
            py + ph > y && py < y + height)
        {
            p.setX(x + width + EPSILON);
            p.setVelocityX(0);
        }
    }
    void draw(RenderWindow &window, float scrollX) override
    {
        sprite.setPosition(x - scrollX, y);
        window.draw(sprite);
    }
};

class Sonic : public player
{
private:
    float boostActivationSpeed;
    bool isOnOpenGround;
    float normalMaxSpeed;
    float boostMaxSpeed;
    float boostAcceleration;
    bool boostReady;

public:
    Sonic(const string &texPath, float scale_x, float scale_y)
        : player(texPath, scale_x, scale_y,
                 acceleration = 0.3f, maxSpeed = 18.f, jump_strength = 22.f),
          isOnOpenGround(false),
          normalMaxSpeed(18.f),
          boostMaxSpeed(40.f),
          boostAcceleration(2.0f), boostActivationSpeed(5.0f),
          boostReady(false)
    {
    }

    void checkOpenGround(char **lvl, int cell_size)
    {
        isOnOpenGround = false;
        boostReady = false;

        if (!onGround || velocityX < boostActivationSpeed)
            return;

        int playerCellX = static_cast<int>((x + width / 2) / cell_size);
        int playerCellY = static_cast<int>((y + height) / cell_size);

        if (lvl[playerCellY + 1][playerCellX] != 'w')
            return;

        bool openPath = true;
        for (int i = 1; i <= 15; ++i)
        {
            if (lvl[playerCellY][playerCellX + i] == 'w' || lvl[playerCellY][playerCellX + i] == 's')
            {
                openPath = false;
                break;
            }
        }

        isOnOpenGround = openPath;
        boostReady = openPath && (velocityX >= boostActivationSpeed);
    }
    void controller() override
    {
        if (boostReady && onGround && velocityX > 0)
        {
            maxSpeed = boostMaxSpeed;
            acceleration = boostAcceleration;
        }
        else
        {
            maxSpeed = normalMaxSpeed;
            acceleration = 0.3f;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            velocityX -= acceleration;
            if (velocityX < -maxSpeed)
                velocityX = -maxSpeed;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            velocityX += acceleration;
            if (velocityX > maxSpeed)
                velocityX = maxSpeed;
        }
        else
        {
            if (velocityX > 0)
            {
                velocityX -= acceleration;
                if (velocityX < 0)
                    velocityX = 0;
            }
            else if (velocityX < 0)
            {
                velocityX += acceleration;
                if (velocityX > 0)
                    velocityX = 0;
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && onGround)
        {
            velocityY = -jump_strength;
            onGround = false;
        }
    }

    void player_gravity(char **lvl, int cell_size) override
    {

        player::player_gravity(lvl, cell_size);
        checkOpenGround(lvl, cell_size);
    }
    bool isSonic() const { return true; }
    bool isTails() const { return false; }
    bool isKnuckles() const { return false; }
};
class Tails : public player
{
protected:
    bool shouldSkipYClamp() const override { return isLookingForLand; }

private:
    bool isFlying;
    float flyTimer;
    float maxFlyTime;
    bool isLookingForLand;
    float flySpeed;
    float minFlightHeight;

public:
    Tails() {}
    Tails(const string &texPath, float scale_x, float scale_y)
        : player(texPath, scale_x, scale_y, 0.15f, 12.f, 25.f),
          isFlying(false),
          flyTimer(0.0f),
          maxFlyTime(7.f),
          isLookingForLand(false),
          flySpeed(30.f),
          minFlightHeight(50.f)
    {
    }

    void startFlying()
    {
        if (!isFlying && !isLookingForLand)
        {
            isFlying = true;
            flyTimer = 0.f;
            velocityY = -flySpeed;
        }
    }

    void stopFlying()
    {
        isFlying = false;
        isLookingForLand = true;
        velocityY = flySpeed;
    }

    void checkForPitfalls(char **lvl, int cell_size)
    {
        if (!isFlying && !isLookingForLand && onGround)
        {
            int playerCellX = static_cast<int>((x + width / 2) / cell_size);
            int playerCellY = static_cast<int>((y + height) / cell_size);

            if (playerCellX + 2 >= width || playerCellY + 1 >= height)
                return;

            if (lvl[playerCellY + 1][playerCellX + 1] == ' ' ||
                lvl[playerCellY + 1][playerCellX + 2] == ' ')
            {
                startFlying();
            }
        }
    }

    void controller() override
    {
        player::controller();

        if (Keyboard::isKeyPressed(Keyboard::Space))
        {
            startFlying();
        }

        if (isFlying)
        {

            if (Keyboard::isKeyPressed(Keyboard::Left))
            {
                velocityX = -flySpeed;
            }
            else if (Keyboard::isKeyPressed(Keyboard::Right))
            {
                velocityX = flySpeed;
            }
            else
            {
                velocityX *= 0.9f;
            }

            if (y > minFlightHeight)
            {
                velocityY = -flySpeed;
            }
            else
            {
                velocityY = 0;
            }
        }
    }

    void update() override
    {
        player::update();

        if (isFlying)
        {
            flyTimer += 1.0f / 60.f;
            if (flyTimer >= maxFlyTime)
            {
                stopFlying();
            }

            if (y < minFlightHeight)
            {
                y = minFlightHeight;
                velocityY = 0;
            }
        }
    }

    void player_gravity(char **lvl, int cell_size) override
    {
        if (isLookingForLand)
        {
            float playerBottom = y + height + hitBoxY;
            int centerX = static_cast<int>((x + width / 2) / cell_size);
            int currentCellY = static_cast<int>(playerBottom / cell_size);
            if (centerX < 0)
                centerX = 0;
            if (centerX >= width)
                centerX = width - 1;
            for (int scanDepth = -1; scanDepth <= 5; ++scanDepth)
            {
                int cellY = currentCellY + scanDepth;
                if (cellY < 0 || cellY >= height)
                    continue;
                for (int dx = -2; dx <= 2; ++dx)
                {
                    int cellX = centerX + dx;
                    if (cellX < 0 || cellX >= width)
                        continue;

                    if (lvl[cellY][cellX] == 'w')
                    {
                        float groundTop = cellY * cell_size;
                        y = groundTop - height - hitBoxY;
                        onGround = true;
                        isLookingForLand = false;
                        isFlying = false;
                        velocityY = 0;
                        flyTimer = 0.f;
                        sprite.setPosition(x, y);
                        return;
                    }
                }
            }
            y += velocityY;
            velocityY += gravity;
            if (velocityY > terminalVelocity)
            {
                velocityY = terminalVelocity;
            }
            sprite.setPosition(x, y);
        }
        else if (!isFlying)
        {
            player::player_gravity(lvl, cell_size);
            checkForPitfalls(lvl, cell_size);
        }
    }
    bool isSonic() const { return false; }
    bool isTails() const { return true; }
    bool isKnuckles() const { return false; }
    bool isFly() const { return isFlying; }
};
class BottomlessPit : public Obstacle
{
public:
    BottomlessPit(float x_, float y_, float w_, float h_)
    {
        x = x_;
        y = y_;
        width = w_;
        height = h_;
    }
    void check(player &p) override;
};
class Platform : public Obstacle
{
    Sprite sprite;

public:
    Platform(float x_, float y_, float w_, float h_, Texture &texture)
    {
        x = x_;
        y = y_;
        width = w_;
        height = h_;
        sprite.setTexture(texture);
        sprite.setScale(float(width) / texture.getSize().x, float(height) / texture.getSize().y);
        sprite.setPosition(x, y);
    }
    void check(player &p) override
    {
        float playerBottom = p.getY() + p.getHeight();
        float playerTop = p.getY();
        float playerLeft = p.getX();
        float playerRight = p.getX() + p.getWidth();
        bool horizontallyOverlapping = playerRight > x && playerLeft < x + width;
        bool fallingOnto = p.getY() < y && playerBottom >= y;
        if (horizontallyOverlapping && fallingOnto)
        {
            p.setY(y - p.getHeight());
            p.setVelocityY(0);
            p.setOnGround(true);
        }
    }
    void draw(RenderWindow &window, float scrollX) override
    {
        sprite.setPosition(x - scrollX, y);
        window.draw(sprite);
    }
};
class Menu
{
private:
    static const int MAX_ITEMS = 4;
    sf::Font font;
    sf::Text menuItems[MAX_ITEMS];
    int selectedIndex;
    sf::Color normalColor;
    sf::Color selectedColor;
    Texture backgroundTexture;
    Sprite backgroundSprite;

public:
    Menu(float width, float height)
        : selectedIndex(0),
          normalColor(sf::Color::White),
          selectedColor(sf::Color::Yellow)
    {
        font.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/NRT-Reg.ttf");
        if (!backgroundTexture.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/bg2.png"))
        {
            cerr << "Failed to load menu background texture!" << endl;
            backgroundSprite.setTextureRect(IntRect(0, 0, 1, 1));
            backgroundSprite.setColor(Color(50, 50, 100));
            backgroundSprite.setScale(width, height);
        }
        else
        {
            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setScale(width / backgroundTexture.getSize().x, height / backgroundTexture.getSize().y);
        }

        const char *option0 = "Level 1";
        const char *option1 = "Level 2";
        const char *option2 = "Level 3";
        const char *option3 = "Boss Level";
        const char *options[MAX_ITEMS] = {option0, option1, option2, option3};

        float verticalSpacing = 80.0f;
        float startY = height / 3.0f;

        for (int i = 0; i < MAX_ITEMS; ++i)
        {
            menuItems[i].setFont(font);
            menuItems[i].setString(options[i]);
            menuItems[i].setCharacterSize(48);
            menuItems[i].setFillColor(i == 0 ? selectedColor : normalColor);
            menuItems[i].setPosition(width / 2.0f - 100.0f, startY + i * verticalSpacing);
        }
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(backgroundSprite);
        for (int i = 0; i < MAX_ITEMS; ++i)
        {
            window.draw(menuItems[i]);
        }
    }

    void moveUp()
    {
        if (selectedIndex > 0)
        {
            menuItems[selectedIndex].setFillColor(normalColor);
            selectedIndex--;
            menuItems[selectedIndex].setFillColor(selectedColor);
        }
    }

    void moveDown()
    {
        if (selectedIndex < MAX_ITEMS - 1)
        {
            menuItems[selectedIndex].setFillColor(normalColor);
            selectedIndex++;
            menuItems[selectedIndex].setFillColor(selectedColor);
        }
    }

    int getSelectedIndex() const
    {
        return selectedIndex;
    }
};
class Spikes : public Obstacle
{

    Sprite sprite;
    bool isPlayerHit;
    const float EPSILON = 0.0001f;

public:
    Spikes(float x_, float y_, float w_, float h_, Texture &texture)
        : isPlayerHit(false)
    {
        x = x_;
        y = y_;
        width = w_;
        height = h_;
        sprite.setTexture(texture);
        sprite.setScale(width / texture.getSize().x, height / texture.getSize().y);
        sprite.setPosition(x, y);
    }

    void check(player &p) override
    {
        if (p.isInvincible())
        {
            return;
        }

        float px = p.getX();
        float py = p.getY();
        float pw = p.getWidth();
        float ph = p.getHeight();
        float pvx = p.getVelocityX();
        float pvy = p.getVelocityY();

        float next_px = px + pvx;
        float next_py = py + pvy;

        float playerLeft = next_px;
        float playerRight = next_px + pw;
        float playerTop = next_py;
        float playerBottom = next_py + ph;
        float spikeLeft = x;
        float spikeRight = x + width;
        float spikeTop = y;
        float spikeBottom = y + height;
        if (playerRight > spikeLeft && playerLeft < spikeRight &&
            playerBottom > spikeTop && playerTop < spikeBottom)
        {

            float penLeft = playerRight - spikeLeft;
            float penRight = spikeRight - playerLeft;
            float penTop = playerBottom - spikeTop;
            float penBottom = spikeBottom - playerTop;

            float minPen = std::min({penLeft, penRight, penTop, penBottom});

            if (minPen == penLeft)
            {
                p.setX(spikeLeft - pw - EPSILON);
                p.setVelocityX(0);
                isPlayerHit = true;
            }
            else if (minPen == penRight)
            {
                p.setX(spikeRight + EPSILON);
                p.setVelocityX(0);
                isPlayerHit = true;
            }
            else if (minPen == penTop)
            {
                p.setY(spikeTop - ph - EPSILON);
                p.setVelocityY(0);
                p.setOnGround(true);
                isPlayerHit = true;
            }
            else if (minPen == penBottom)
            {
                p.setY(spikeBottom + EPSILON);
                p.setVelocityY(0);
                isPlayerHit = true;
            }

            if (isPlayerHit)
            {

                p.setVelocityY(-abs(p.getVelocityY()) * 0.5f);
            }
        }
        else
        {
            isPlayerHit = false;
        }
    }

    void draw(RenderWindow &window, float scrollX) override
    {
        sprite.setPosition(x - scrollX, y);
        window.draw(sprite);
    }
};
void markObstacle(char **lvl, int x, int y, int width, int height, char symbol, int cell_size)
{
    int start_col = x / cell_size;
    int start_row = y / cell_size;
    int end_col = (x + width) / cell_size;
    int end_row = (y + height) / cell_size;

    for (int row = start_row; row < end_row; ++row)
    {
        for (int col = start_col; col < end_col; ++col)
        {
            if (row >= 0 && row < 14 && col >= 0 && col < 110)
                lvl[row][col] = symbol;
        }
    }
}
class Enemy
{
protected:
    Texture texture;
    Sprite sprite;
    float x, y;

    float terminalVelocity;

    int rawWidth, rawHeight;
    float width, height;
    float hitBoxX, hitBoxY;

public:
    Enemy(const string &texPath, float scale_x, float scale_y, float accel = 0.2f) : x(100), y(100), rawWidth(24), rawHeight(35)
    {
        texture.loadFromFile(texPath);
        sprite.setTexture(texture);
        sprite.setScale(scale_x, scale_y);

        width = rawWidth * scale_x;
        height = rawHeight * scale_y;
        hitBoxX = 8 * scale_x;
        hitBoxY = 5 * scale_y;

        sprite.setPosition(x, y);
    }
    virtual bool checkCollisionWithPlayer(float px, float py, float pwidth, float pheight, float scrollX) = 0;
    virtual ~Enemy() {};
    virtual void update(float px, float py, float deltaTime, class Level *level) = 0;
    virtual void draw(sf::RenderWindow &window, float scrollX) = 0;
    virtual void takeDamage(int amount) = 0;
    virtual bool isDead() const = 0;
};
class BeeBot : public Enemy
{
private:
    int directionX;
    int directionY;

    float velocityX;
    float velocityY;
    int hp;

    float minY, maxY;
    float fixedStartX, rangeWidth;

    sf::Texture projectileTexture;
    sf::Sprite projectiles[10];
    float projectileX[10], projectileY[10];
    float projectileVX[10], projectileVY[10];
    bool projectileActive[10];
    int projectileCount;
    float projectileSpeed;

    float shootTimer;
    float shootInterval;

public:
    BeeBot(const string &texPath, const string &projectileTexPath, float startX, float startY)
        : Enemy(texPath, 2.5f, 2.5f),
          directionX(1), directionY(1),
          velocityX(2.f), velocityY(2.f),
          projectileCount(0), projectileSpeed(5.f),
          shootTimer(0.f), shootInterval(3.f),
          minY(100.f), maxY(396.f),
          fixedStartX(startX), rangeWidth(350.f)
    {
        x = startX;
        y = startY;
        sprite.setPosition(x, y);
        hp = 2;

        projectileTexture.loadFromFile(projectileTexPath);
        for (int i = 0; i < 10; i++)
        {
            projectiles[i].setTexture(projectileTexture);
            projectiles[i].setScale(0.5f, 0.5f);
            projectileActive[i] = false;
        }
    }
    void takeDamage(int damage)
    {
        hp -= damage;
        if (hp < 0)
            hp = 0;
    }
    bool isDead() const override
    {
        return hp <= 0;
    }
    void update(float px, float py, float deltaTime, Level *level) override
    {
        x += directionX * velocityX;
        y += directionY * velocityY;
        if (x <= fixedStartX || x + width >= fixedStartX + rangeWidth)
        {
            directionX *= -1;
        }
        if (y <= minY)
        {
            y = minY;
            directionY = 1;
        }
        if (y + height >= maxY)
        {
            y = maxY - height;
            directionY = -1;
        }
        sprite.setPosition(x, y);
        shootTimer += 1.f / 60.f;
        if (shootTimer >= shootInterval && projectileCount < 10)
        {
            float projStartX = x + width / 2;
            float projStartY = y + height / 2;

            float dx = px - projStartX;
            float dy = py - projStartY;
            float length = sqrt(dx * dx + dy * dy);
            projectileVX[projectileCount] = (dx / length) * projectileSpeed;
            projectileVY[projectileCount] = (dy / length) * projectileSpeed;

            projectileX[projectileCount] = projStartX;
            projectileY[projectileCount] = projStartY;
            projectiles[projectileCount].setPosition(projStartX, projStartY);
            projectileActive[projectileCount] = true;

            projectileCount++;
            shootTimer = 0.f;
        }
        for (int i = 0; i < projectileCount; i++)
        {
            if (projectileActive[i])
            {
                projectileX[i] += projectileVX[i];
                projectileY[i] += projectileVY[i];
                projectiles[i].setPosition(projectileX[i], projectileY[i]);

                if (projectileX[i] < 0 || projectileX[i] > screen_x || projectileY[i] > screen_y || projectileY[i] < 0)
                {
                    projectileActive[i] = false;
                }
            }
        }
    }
    void draw(sf::RenderWindow &window, float scrollX) override
    {
        sprite.setPosition(x - scrollX, y);
        window.draw(sprite);

        for (int i = 0; i < projectileCount; i++)
        {
            if (projectileActive[i])
            {
                projectiles[i].setPosition(projectileX[i] - scrollX, projectileY[i]);
                window.draw(projectiles[i]);
            }
        }
    }
    bool checkCollisionWithPlayer(float px, float py, float pwidth, float pheight, float scrollX) override
    {
        if (px + pwidth > x - scrollX &&
            px < x + width - scrollX &&
            py + pheight > y &&
            py < y + height)
        {
            return true;
        }
        for (int i = 0; i < projectileCount; i++)
        {
            if (projectileActive[i])
            {
                float projSize = 40.0f;
                if (px + pwidth > projectileX[i] - scrollX &&
                    px < projectileX[i] + projSize - scrollX &&
                    py + pheight > projectileY[i] &&
                    py < projectileY[i] + projSize)
                {
                    projectileActive[i] = false;
                    return true;
                }
            }
        }
        return false;
    }
};
class CrabMeat : public Enemy
{
private:
    int directionX;
    float moveSpeed;
    float patrolStartX;
    float patrolEndX;
    int hp;
    float originalX;

    sf::Texture projectileTexture;
    sf::Sprite projectiles[5];
    float projectileX[5], projectileY[5];
    bool projectileActive[5];
    int projectileCount;
    float projectileSpeed;

    float shootTimer;
    float shootInterval;

public:
    CrabMeat(const string &texPath, const string &projectileTexPath, float startX, float startY, float patrolStart, float patrolEnd)
        : Enemy(texPath, 2.5f, 2.5f),
          directionX(1), moveSpeed(1.5f), hp(4),
          patrolStartX(patrolStart), patrolEndX(patrolEnd),
          projectileCount(0), projectileSpeed(4.f),
          shootTimer(0.f), shootInterval(3.f),
          originalX(startX)
    {
        x = startX;
        y = startY;
        sprite.setPosition(x, y);

        projectileTexture.loadFromFile(projectileTexPath);
        for (int i = 0; i < 5; i++)
        {
            projectiles[i].setTexture(projectileTexture);
            projectiles[i].setScale(0.5f, 0.5f);
            projectileActive[i] = false;
        }
    }

    void update(float px = 0, float py = 0, float deltaTime = 1.f / 60.f, Level *level = nullptr) override
    {

        originalX += directionX * moveSpeed;

        if (originalX <= patrolStartX)
        {
            directionX = 1;
        }
        else if (originalX + width >= patrolEndX)
        {
            directionX = -1;
        }

        shootTimer += 1.f / 60.f;
        if (shootTimer >= shootInterval && projectileCount < 5)
        {
            projectileX[projectileCount] = x + width / 2;
            projectileY[projectileCount] = y + height / 2;
            projectiles[projectileCount].setPosition(projectileX[projectileCount], projectileY[projectileCount]);
            projectileActive[projectileCount] = true;
            projectileCount++;
            shootTimer = 0.f;
        }

        for (int i = 0; i < projectileCount; i++)
        {
            if (projectileActive[i])
            {
                projectileX[i] += directionX * projectileSpeed;
                projectiles[i].setPosition(projectileX[i], projectileY[i]);
                if (projectileX[i] < 0 || projectileX[i] > screen_x)
                {
                    projectileActive[i] = false;
                }
            }
        }
    }

    void draw(sf::RenderWindow &window, float scrollX)
    {

        x = originalX - scrollX;
        sprite.setPosition(x, y);
        window.draw(sprite);

        for (int i = 0; i < projectileCount; i++)
        {
            if (projectileActive[i])
            {
                window.draw(projectiles[i]);
            }
        }
    }

    bool checkCollisionWithPlayer(float px, float py, float pwidth, float pheight, float scrollX) override
    {
        float crabWorldX = originalX - scrollX;
        if (px + pwidth > crabWorldX &&
            px < crabWorldX + width &&
            py + pheight > y &&
            py < y + height)
        {
            return true;
        }
        const float projectileWidth = 80.0f;
        const float projectileHeight = 32.0f;

        for (int i = 0; i < projectileCount; i++)
        {
            if (projectileActive[i])
            {
                if (px + pwidth > projectileX[i] - scrollX &&
                    px < projectileX[i] + projectileWidth - scrollX &&
                    py + pheight > projectileY[i] &&
                    py < projectileY[i] + projectileHeight)
                {
                    projectileActive[i] = false;
                    return true;
                }
            }
        }
        return false;
    }

    void takeDamage(int damage)
    {
        hp -= damage;
        if (hp < 0)
            hp = 0;
    }
    bool isDead() const override { return hp <= 0; }
};
class Motobug : public Enemy
{
private:
    int direction;
    float speed;
    float patrolMinX, patrolMaxX;
    float activationRange;
    int hp;
    bool isActive;

public:
    Motobug(const string &texPath, float startX, float startY,
            float crawlSpeed = 2.f, float detectRange = 200.f)
        : Enemy(texPath, 2.5f, 2.5f),
          direction(1),
          speed(crawlSpeed),
          activationRange(detectRange),
          hp(2),
          isActive(false)
    {
        x = startX;
        y = startY;
        sprite.setPosition(x, y);

        patrolMinX = startX - 150.f;
        patrolMaxX = startX + 150.f;
    }

    void update(float px, float py, float deltaTime = 1.f / 60.f, Level *level = nullptr) override
    {
        if (hp <= 0)
            return;

        float dx = px - x;
        float dy = py - y;
        float distance = sqrt(dx * dx + dy * dy);

        isActive = (distance <= activationRange);

        if (isActive)
        {
            direction = (dx > 0 ? 1 : -1);
            float nextX = x + direction * speed;
            if (nextX >= patrolMinX && nextX + width <= patrolMaxX)
            {
                x = nextX;
            }
        }
        else
        {
            float nextX = x + direction * speed;
            if (nextX <= patrolMinX || nextX + width >= patrolMaxX)
            {
                direction *= -1;
            }
            x += direction * speed;
        }
        sprite.setPosition(x, y);
    }
    bool checkCollisionWithPlayer(float px, float py, float pwidth, float pheight, float scrollX) override
    {
        if (hp <= 0)
            return false;

        float screenX = x - scrollX;
        return (px + pwidth > screenX &&
                px < screenX + width &&
                py + pheight > y &&
                py < y + height);
    }

    void takeDamage(int damage = 1)
    {
        hp -= damage;
        if (hp < 0)
            hp = 0;
    }

    bool isDead() const
    {
        return hp <= 0;
    }

    void draw(sf::RenderWindow &window, float scrollX) override
    {
        if (hp <= 0)
            return;
        sprite.setPosition(x - scrollX, y);
        window.draw(sprite);
    }

    void placeMotobug(char **lvl, int rows, int cols, int cellSize)
    {

        float levelCenterX = (cols * cellSize) / 2.0f - width / 2.0f;

        int centerCol = static_cast<int>(levelCenterX / cellSize);
        if (centerCol < 0)
            centerCol = 0;
        if (centerCol >= cols)
            centerCol = cols - 1;
        for (int row = 0; row < rows; row++)
        {
            if (lvl[row][centerCol] == 'w')
            {
                x = levelCenterX;
                y = row * cellSize - height;
                patrolMinX = x - 150.f;
                patrolMaxX = x + 150.f;

                sprite.setPosition(x, y);
                return;
            }
        }
        x = levelCenterX;
        y = rows * cellSize - height;
        sprite.setPosition(x, y);
    }
};

class BatBrain : public Enemy
{
    sf::Texture tex;
    sf::Sprite sprite;
    int currentState;
    int hp;
    float speed;
    float floatTimer;
    float baseY;
    bool isAlive;
    float homeX, homeY;
    float returnDelayTimer;
    const float returnDelay = 0.5f;
    float collisionCooldown;

public:
    BatBrain(const char *texPath, float sx, float sy, float moveSpeed = 0.5f)
        : Enemy(texPath, 2.5f, 2.5f), speed(moveSpeed), floatTimer(0),
          baseY(sy), isAlive(true), homeX(sx), homeY(sy),
          returnDelayTimer(0), collisionCooldown(0), currentState(0)
    {

        x = sx;
        y = sy;
        hp = 3;

        if (tex.loadFromFile(texPath))
        {
            sprite.setTexture(tex);
            width = static_cast<float>(tex.getSize().x);
            height = static_cast<float>(tex.getSize().y);
            sprite.setPosition(x, y);
            sprite.setOrigin(width / 2, height / 2);
        }
    }
    void takeDamage(int damage)
    {
        hp -= damage;
        if (hp < 0)
            hp = 0;
    }
    bool isDead() const override { return hp <= 0; }

    void update(float px, float py, float deltaTime = 1.f / 60.f, Level *level = nullptr) override
    {
        if (!isAlive)
            return;
        floatTimer += deltaTime;
        if (collisionCooldown > 0)
            collisionCooldown -= deltaTime;
        float dx = px - x;
        float dy = py - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist > 1.0f)
        {
            float approachSpeed = speed + (1.5f / (dist + 1.0f));
            float vx = dx / dist * approachSpeed * deltaTime * 60.0f;
            float vy = dy / dist * approachSpeed * deltaTime * 60.0f;

            x += vx;
            y += vy * 0.7f;
        }
        float floatOffset = std::sin(floatTimer * 2.0f) * 10.0f;
        sprite.setPosition(x, y + floatOffset);
        sprite.setScale((dx >= 0) ? 1.f : -1.f, 1.f);
    }

    bool checkCollisionWithPlayer(float px, float py, float pwidth, float pheight, float scrollX) override
    {
        if (!isAlive)
            return false;

        float screenX = x - scrollX;
        float floatOffset = std::sin(floatTimer * 2.0f) * 10.0f;
        float actualY = y + floatOffset;

        return (px + pwidth > screenX &&
                px < screenX + width &&
                py + pheight > actualY &&
                py < actualY + height);
    }

    void draw(sf::RenderWindow &window, float scrollX)
    {
        if (!isAlive)
            return;
        sprite.setPosition(x - scrollX, y);
        window.draw(sprite);
    }
};

class EnemyFactory
{
private:
    Enemy **enemies;
    int size;
    int capacity;

public:
    EnemyFactory(int initialCapacity = 5) : size(0), capacity(initialCapacity)
    {
        enemies = new Enemy *[capacity];
    }
    ~EnemyFactory()
    {
        for (int i = 0; i < size; ++i)
        {
            delete enemies[i];
        }
        delete[] enemies;
    }
    void addEnemy(Enemy *p)
    {
        if (size >= capacity)
        {
            capacity *= 2;
            Enemy **newEnemies = new Enemy *[capacity];
            for (int i = 0; i < size; ++i)
            {
                newEnemies[i] = enemies[i];
            }
            delete[] enemies;
            enemies = newEnemies;
        }
        enemies[size] = p;
        size++;
    }
    void initializeDefaults(float scaleX, float scaleY, char **lvlData, int cellSize)
    {
        addEnemy(new CrabMeat("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0right_still.png",
                              "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
                              1000, 600, 1000, 1500));
        addEnemy(new BeeBot("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png", "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png", 1100.f, 50.f));

        addEnemy(new BatBrain("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png", 200.f, 400.f));
        Motobug *motobug = new Motobug(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            1200.f, 600.f, 2.f, 200.f);
        motobug->placeMotobug(lvlData, 14, 200, cellSize);
        addEnemy(motobug);
    }
    Enemy *getEnemy(int index)
    {
        if (index >= 0 && index < size)
        {
            return enemies[index];
        }
        else
        {
            return nullptr;
        }
    }
    int getSize() const
    {
        return size;
    }
};
class Collectable
{
protected:
    float x, y;
    float width, height;
    Sprite sprite;
    bool collected;

public:
    Collectable(float x_, float y_, float w, float h, const Texture &tex)
        : x(x_), y(y_), width(w), height(h), collected(false)
    {
        sprite.setTexture(tex);
        sprite.setScale(width / tex.getSize().x, height / tex.getSize().y);
        sprite.setPosition(x, y);
    }
    virtual void draw(RenderWindow &window, float scrollX)
    {
        if (!collected)
        {
            sprite.setPosition(x - scrollX, y);
            window.draw(sprite);
        }
    }

    virtual bool checkCollision(player *currentPlayer)
    {
        if (collected)
            return false;
        float px = currentPlayer->getX();
        float py = currentPlayer->getY();
        float pWidth = currentPlayer->getWidth();
        float pHeight = currentPlayer->getHeight();
        if (px + pWidth > x && px < x + width &&
            py + pHeight > y && py < y + height)
        {
            collected = true;
            return true;
        }
        return false;
    }

    bool isCollected() const { return collected; }
    void reset() { collected = false; }

    virtual ~Collectable() {}
};
class Ring : public Collectable
{
private:
    Clock animationClock;
    int currentFrame;
    const int FRAME_COUNT = 4;
    const float FRAME_DURATION = 0.1f;

public:
    Ring(float x_, float y_, float w, float h, const Texture &tex)
        : Collectable(x_, y_, w, h, tex), currentFrame(0)
    {
        sprite.setTextureRect(IntRect(0, 0, tex.getSize().x / FRAME_COUNT, tex.getSize().y));
        sprite.setScale(width / (tex.getSize().x / FRAME_COUNT), height / tex.getSize().y);
    }

    void draw(RenderWindow &window, float scrollX) override
    {
        if (!collected)
        {
            sprite.setPosition(x - scrollX, y);
            window.draw(sprite);
        }
    }
};
class SpecialBoost : public Collectable
{
private:
    Clock effectTimer;
    bool isActive;
    const float DURATION = 15.0f;
    player *boostedPlayer;

    void applyEffect()
    {
        if (boostedPlayer->isSonic())
        {
            boostedPlayer->setMaxSpeed(22.f);
        }
        else if (boostedPlayer->isTails())
        {
            boostedPlayer->increaseFlightTime(2.0f);
        }
        else if (boostedPlayer->isKnuckles())
        {
            boostedPlayer->setInvincible(true);
        }
    }
    void removeEffect()
    {
        if (boostedPlayer->isSonic())
        {
            boostedPlayer->setMaxSpeed(18.f);
        }
        else if (boostedPlayer->isTails())
        {
            boostedPlayer->resetFlightTime();
        }
        else if (boostedPlayer->isKnuckles())
        {
            boostedPlayer->setInvincible(false);
        }
    }

public:
    SpecialBoost(float x_, float y_, float w, float h, const Texture &tex)
        : Collectable(x_, y_, w, h, tex), isActive(false), boostedPlayer(nullptr)
    {
    }

    void draw(RenderWindow &window, float scrollX) override
    {
        if (!collected)
        {
            sprite.setPosition(x - scrollX, y);
            window.draw(sprite);
        }
    }
    bool checkCollision(player *currentPlayer)
    {
        if (collected || isActive)
            return false;

        float px = currentPlayer->getX();
        float py = currentPlayer->getY();
        float pw = currentPlayer->getWidth();
        float ph = currentPlayer->getHeight();

        if (px + pw > x && px < x + width &&
            py + ph > y && py < y + height)
        {
            collected = true;
            isActive = true;
            boostedPlayer = currentPlayer;
            effectTimer.restart();
            applyEffect();
            return true;
        }
        return false;
    }
    bool activateBoostIfCollected(player *currentPlayer)
    {
        if (collected && !isActive && boostedPlayer == nullptr)
        {
            boostedPlayer = currentPlayer;
            effectTimer.restart();
            isActive = true;
            return true;
        }
        return false;
    }

    void update()
    {
        if (isActive && boostedPlayer != nullptr)
        {
            if (effectTimer.getElapsedTime().asSeconds() > DURATION)
            {
                removeEffect();
                isActive = false;
                boostedPlayer = nullptr;
            }
            else
            {
                applyEffect();
            }
        }
    }
};
class ExtraLife : public Collectable
{
public:
    ExtraLife(float x_, float y_, float w, float h, const Texture &tex)
        : Collectable(x_, y_, w, h, tex)
    {
    }
};

class Level
{
private:
    char **lvlData;
    int width;
    int height;
    int cellSize;
    Obstacle **obstacles;
    int obstacleCount;
    int maxObstacles;
    Collectable **collectables;
    int collectableCount;
    int maxCollectables;
    Enemy **enemies;
    int enemyCount;
    int maxEnemies;
    Texture backgroundTexture;
    Sprite backgroundSprite;
    Music levelMusic;
    Texture wallTexture;
    Sprite wallSprite;
    std::string name;

public:
    Level(int w, int h, int cs, int maxObs, int maxCol, int maxEn, const std::string &levelName)
        : width(w), height(h), cellSize(cs), maxObstacles(maxObs), maxCollectables(maxCol), maxEnemies(maxEn), name(levelName)
    {

        lvlData = new char *[height];
        for (int i = 0; i < height; ++i)
        {
            lvlData[i] = new char[width]{'\0'};
        }
        obstacles = new Obstacle *[maxObstacles];
        obstacleCount = 0;

        collectables = new Collectable *[maxCollectables];
        collectableCount = 0;

        enemies = new Enemy *[maxEnemies];
        enemyCount = 0;
    }

    ~Level()
    {
        for (int i = 0; i < height; ++i)
        {
            delete[] lvlData[i];
        }
        delete[] lvlData;
        for (int i = 0; i < obstacleCount; ++i)
        {
            delete obstacles[i];
        }
        delete[] obstacles;
        for (int i = 0; i < collectableCount; ++i)
        {
            delete collectables[i];
        }
        delete[] collectables;
        for (int i = 0; i < enemyCount; ++i)
        {
            delete enemies[i];
        }
        delete[] enemies;
    }

    void setBackground(const std::string &texturePath)
    {
        backgroundTexture.loadFromFile(texturePath);
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(float(screen_x) / backgroundTexture.getSize().x,
                                  float(screen_y) / backgroundTexture.getSize().y);
    }

    void setWallTexture(const std::string &texturePath)
    {
        wallTexture.loadFromFile(texturePath);
        wallSprite.setTexture(wallTexture);
    }

    void setMusic(const std::string &musicPath)
    {
        levelMusic.openFromFile(musicPath);
        levelMusic.setVolume(30);
        levelMusic.setLoop(true);
    }

    void playMusic()
    {
        levelMusic.play();
    }

    void stopMusic()
    {
        levelMusic.stop();
    }

    void addObstacle(Obstacle *obs)
    {
        if (obstacleCount < maxObstacles)
        {
            obstacles[obstacleCount++] = obs;
        }
    }
    void removeObstacleAt(int x, int y)
    {
        for (int i = 0; i < obstacleCount; ++i)
        {
            if (obstacles[i]->getX() <= x && obstacles[i]->getX() + obstacles[i]->getWidth() > x &&
                obstacles[i]->getY() <= y && obstacles[i]->getY() + obstacles[i]->getHeight() > y)
            {
                delete obstacles[i];

                for (int j = i; j < obstacleCount - 1; ++j)
                {
                    obstacles[j] = obstacles[j + 1];
                }
                obstacleCount--;
                break;
            }
        }
    }

    void addCollectable(Collectable *col)
    {
        if (collectableCount < maxCollectables)
        {
            collectables[collectableCount++] = col;
        }
    }

    void addEnemy(Enemy *enemy)
    {
        if (enemyCount < maxEnemies)
        {
            enemies[enemyCount++] = enemy;
        }
    }

    void setCell(int row, int col, char value)
    {
        if (row >= 0 && row < height && col >= 0 && col < width)
        {
            lvlData[row][col] = value;
        }
    }

    void markObstacle(int x, int y, int w, int h, char symbol)
    {
        int start_col = x / cellSize;
        int start_row = y / cellSize;
        int end_col = (x + w) / cellSize;
        int end_row = (y + h) / cellSize;

        for (int row = start_row; row < end_row; ++row)
        {
            for (int col = start_col; col < end_col; ++col)
            {
                if (row >= 0 && row < height && col >= 0 && col < width)
                {
                    lvlData[row][col] = symbol;
                }
            }
        }
    }

    void draw(RenderWindow &window, float scrollX)
    {
        window.draw(backgroundSprite);
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (lvlData[i][j] == 'w')
                {
                    wallSprite.setPosition(j * cellSize - scrollX, i * cellSize);
                    window.draw(wallSprite);
                }
            }
        }
        for (int i = 0; i < obstacleCount; ++i)
        {
            obstacles[i]->draw(window, scrollX);
        }

        for (int i = 0; i < collectableCount; ++i)
        {
            collectables[i]->draw(window, scrollX);
        }

        for (int i = 0; i < enemyCount; ++i)
        {
            enemies[i]->draw(window, scrollX);
        }
    }

    void update(player *p, float dt)
    {
        for (int i = 0; i < enemyCount; i++)
        {
            enemies[i]->update(p->getX(), p->getY(), dt, this);
        }
    }
    void checkCollisions(player *currentPlayer)
    {
        for (int i = 0; i < obstacleCount; ++i)
        {
            obstacles[i]->check(*currentPlayer);
        }

        for (int i = 0; i < collectableCount; ++i)
        {
            SpecialBoost *boost = dynamic_cast<SpecialBoost *>(collectables[i]);
            if (boost)
            {
                boost->update();
                if (boost->checkCollision(currentPlayer))
                {

                    delete collectables[i];
                    for (int j = i; j < collectableCount - 1; j++)
                    {
                        collectables[j] = collectables[j + 1];
                    }
                    collectableCount--;
                    i--;
                    continue;
                }
            }
            else if (!boost)
            {
                if (collectables[i]->checkCollision(currentPlayer))
                {

                    delete collectables[i];
                    for (int j = i; j < collectableCount - 1; j++)
                    {
                        collectables[j] = collectables[j + 1];
                    }
                    collectableCount--;
                    i--;
                }
            }
        }

        if (!(currentPlayer->isInvincible()))
        {
            for (int i = 0; i < enemyCount; ++i)
            {
                if (enemies[i]->checkCollisionWithPlayer(
                        currentPlayer->getX(),
                        currentPlayer->getY(),
                        currentPlayer->getWidth(),
                        currentPlayer->getHeight(), 0))
                {
                    bool stomping = currentPlayer->getVelocityY() > 0 && !currentPlayer->getOnGround();
                    if (stomping)
                    {
                        enemies[i]->takeDamage(1);
                        float bounceStrength = -currentPlayer->getVelocityY() * 0.5f;
                        currentPlayer->setVelocityY(bounceStrength);
                        if (enemies[i]->isDead())
                        {
                            delete enemies[i];
                            for (int j = i; j + 1 < enemyCount; ++j)
                                enemies[j] = enemies[j + 1];
                            --enemyCount;
                            --i;
                        }
                    }
                    else
                    {
                        player::decreaseHP();
                    }
                    break;
                }
            }
        }
    }

    const std::string &getName() const { return name; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getCellSize() const { return cellSize; }
    char **getLevelData() const { return lvlData; }
    int getCollectableCount() const
    {
        return collectableCount;
    }

    Collectable **getCollectables()
    {
        return collectables;
    }
    Obstacle **getObstacles() { return obstacles; }
    int &getObstacleCount() { return obstacleCount; }
};
class Knuckles : public player
{
public:
    Knuckles(const string &texPath, float scale_x, float scale_y)
        : player(texPath, scale_x, scale_y,
                 acceleration = 0.25f, maxSpeed = 14.f, jump_strength = 20.f)
    {
    }
    bool isSonic() const { return false; }
    bool isTails() const { return false; }
    bool isKnuckles() const override { return true; }
    void breakWalls(Level *level, Obstacle *obs[], int &count, float range)
    {
        for (int i = 0; i < count; ++i)
        {
            if (obs[i]->isWall())
            {
                Wall *w = static_cast<Wall *>(obs[i]);
                float px = getX(), py = getY(), pw = getWidth(), ph = getHeight();
                float feetY = py + ph * 0.9f;
                if (feetY < w->getY() || feetY > w->getY() + w->getHeight())
                    continue;
                float punchX = px + pw + range;
                if (punchX >= w->getX() && punchX <= w->getX() + w->getWidth())
                {
                    int cellSize = level->getCellSize();
                    int startRow = w->getY() / cellSize;
                    int startCol = w->getX() / cellSize;
                    int endRow = (w->getY() + w->getHeight()) / cellSize;
                    int endCol = (w->getX() + w->getWidth()) / cellSize;
                    for (int row = startRow; row < endRow; ++row)
                    {
                        for (int col = startCol; col < endCol; ++col)
                        {
                            if (row >= 0 && row < level->getHeight() && col >= 0 && col < level->getWidth())
                            {
                                level->setCell(row, col, ' ');
                            }
                        }
                    }
                    delete obs[i];
                    for (int j = i; j + 1 < count; ++j)
                        obs[j] = obs[j + 1];
                    --count;
                    --i;
                }
            }
        }
    }
};
class PlayerFactory
{
private:
    player **players;
    int size;
    int capacity;

public:
    PlayerFactory(int initialCapacity = 3) : size(0), capacity(initialCapacity)
    {
        players = new player *[capacity];
    }

    ~PlayerFactory()
    {
        for (int i = 0; i < size; ++i)
            delete players[i];
        delete[] players;
    }
    void addPlayer(player *p)
    {
        if (size >= capacity)
        {
            capacity *= 2;
            player **newPlayers = new player *[capacity];
            for (int i = 0; i < size; ++i)
            {
                newPlayers[i] = players[i];
            }
            delete[] players;
            players = newPlayers;
        }
        players[size] = p;
        size++;
    }

    void initializeDefaults(float scaleX, float scaleY)
    {
        addPlayer(new Sonic("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0right_still.png", scaleX, scaleY));
        addPlayer(new Tails("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/tails.jpg", scaleX, scaleY));
        addPlayer(new Knuckles("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/knuck.png", scaleX, scaleY));
    }
    player *getPlayer(int index)
    {
        if (index >= 0 && index < size)
        {
            return players[index];
        }
        else
        {
            return nullptr;
        }
    }
    int getSize() const
    {
        return size;
    }
};
class EggStinger : public Enemy
{
private:
    int hp;
    float speedX;
    bool movingRight;
    float attackCooldown;
    float attackTimer;
    bool isDiving;
    float diveSpeedY;
    float initialY;

public:
    EggStinger(const std::string &texPath, float scale_x, float scale_y)
        : Enemy(texPath, scale_x, scale_y), hp(15), speedX(200.f), movingRight(true),
          attackCooldown(10.f), attackTimer(0.f), isDiving(false), diveSpeedY(300.f)
    {
        initialY = y;
        terminalVelocity = 400.f;
    }

    bool checkCollisionWithPlayer(float px, float py, float pwidth, float pheight, float scrollX) override
    {

        float ex = x - scrollX;
        float ey = y;

        float ewidth = width;
        float eheight = height;

        if (px < ex + ewidth && px + pwidth > ex &&
            py < ey + eheight && py + pheight > ey)
        {
            return true;
        }
        return false;
    }

    void takeDamage(int damage)
    {
        hp -= damage;
        if (hp < 0)
            hp = 0;
    }

    bool isDead() const override
    {
        return hp <= 0;
    }

    void update(float px, float py, float deltaTime, Level *level) override
    {

        attackTimer += deltaTime;

        if (!isDiving)
        {
            if (movingRight)
            {
                x += speedX * deltaTime;
                if (x >= level->getWidth() * level->getCellSize() - width)
                {
                    movingRight = false;
                }
            }
            else
            {
                x -= speedX * deltaTime;
                if (x <= 0)
                {
                    movingRight = true;
                }
            }
            if (attackTimer >= attackCooldown &&
                abs((x + width / 2) - (px + width / 2)) < 10)
            {
                isDiving = true;
                attackTimer = 0.f;
            }
        }
        else
        {
            y += diveSpeedY * deltaTime;
            if (y >= 11 * level->getCellSize() - height)
            {
                y = 11 * level->getCellSize() - height;
                if (level)
                {
                    int cellX = static_cast<int>(x / level->getCellSize());
                    level->setCell(11, cellX, ' ');
                    level->removeObstacleAt(cellX * level->getCellSize(), 11 * level->getCellSize());
                }
                isDiving = false;
                y = initialY;
            }
        }

        sprite.setPosition(x, y);
    }

    void draw(sf::RenderWindow &window, float scrollX) override
    {
        sprite.setPosition(x - scrollX, y);
        window.draw(sprite);
    }

    int getHP() const { return hp; }
};

class LevelFactory
{
private:
    static const int MAX_LEVELS = 4;
    Level *levels[MAX_LEVELS];
    int levelCount;
    Texture wallTex1;
    Texture platformTex;
    Texture spikesTex;
    Texture boostTex;
    Texture lifeTex;
    Texture ringTex;

public:
    LevelFactory() : levelCount(0)
    {

        wallTex1.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/brick1.png");
        platformTex.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/brick2.png");
        spikesTex.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/spike.png");
        boostTex.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/brick3.png");
        lifeTex.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/brick2.png");
        ringTex.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png");

        createLevel1();
        createLevel2();
        createLevel3();
        createBossLevel();
    }
    ~LevelFactory()
    {
        for (int i = 0; i < levelCount; ++i)
        {
            delete levels[i];
        }
    }
    Level *getLevel(int index)
    {
        if (index >= 0 && index < levelCount)
        {
            return levels[index];
        }
        return nullptr;
    }

    int getLevelCount() const
    {
        return levelCount;
    }

private:
    void createLevel1()
    {
        const int width = 200;
        const int height = 14;
        const int cellSize = 64;
        const int maxObstacles = 40;
        const int maxCollectables = 60;
        const int maxEnemies = 10;

        Level *level = new Level(width, height, cellSize, maxObstacles, maxCollectables, maxEnemies, "Level 1");

        level->setBackground("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/bg1.png");
        level->setWallTexture("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/brick1.png");
        level->setMusic("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/labrynth.ogg");

        for (int i = 0; i < height; ++i)
        {
            if (i == 11)
            {
                for (int j = 0; j < width; j++)
                {
                    if (j >= 60 && j <= 70)
                        level->setCell(i, j, ' ');
                    else
                        level->setCell(i, j, 'w');
                }
            }
        }

        level->addObstacle(new Platform(5 * cellSize, 9 * cellSize, 5 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(6 * cellSize, 8 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(7 * cellSize, 8 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(8 * cellSize, 8 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new SpecialBoost(9 * cellSize, 8 * cellSize, cellSize, cellSize, boostTex));
        level->addEnemy(new CrabMeat(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0right_still.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            20 * cellSize, 10 * cellSize, 20 * cellSize, 30 * cellSize));
        level->addEnemy(new Motobug(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            35 * cellSize, 10 * cellSize));
        level->addObstacle(new Spikes(15 * cellSize, 10 * cellSize, cellSize, cellSize, spikesTex));
        level->markObstacle(15 * cellSize, 10 * cellSize, cellSize, cellSize, 's');
        level->addObstacle(new Platform(25 * cellSize, 8 * cellSize, 4 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(26 * cellSize, 7 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(27 * cellSize, 7 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(28 * cellSize, 7 * cellSize, cellSize, cellSize, ringTex));

        level->addObstacle(new BottomlessPit(60 * cellSize, 11 * cellSize, 11 * cellSize, 1 * cellSize));
        level->markObstacle(60 * cellSize, 11 * cellSize, 11 * cellSize, 1 * cellSize, ' ');
        level->addCollectable(new Ring(61 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(62 * cellSize, 5 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(63 * cellSize, 4 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(64 * cellSize, 3 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(65 * cellSize, 4 * cellSize, cellSize, cellSize, ringTex));
        level->addEnemy(new BeeBot(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            64 * cellSize, 2 * cellSize));

        level->addObstacle(new Wall(90 * cellSize, 10 * cellSize, cellSize, cellSize, wallTex1));
        level->markObstacle(90 * cellSize, 10 * cellSize, cellSize, cellSize, 'w');
        level->addObstacle(new Wall(91 * cellSize, 10 * cellSize, cellSize, cellSize, wallTex1));
        level->markObstacle(91 * cellSize, 10 * cellSize, cellSize, cellSize, 'w');
        level->addObstacle(new Wall(92 * cellSize, 10 * cellSize, cellSize, cellSize, wallTex1));
        level->markObstacle(92 * cellSize, 10 * cellSize, cellSize, cellSize, 'w');
        level->addObstacle(new Wall(90 * cellSize, 9 * cellSize, cellSize, cellSize, wallTex1));
        level->markObstacle(90 * cellSize, 9 * cellSize, cellSize, cellSize, 'w');
        level->addObstacle(new Wall(91 * cellSize, 9 * cellSize, cellSize, cellSize, wallTex1));
        level->markObstacle(91 * cellSize, 9 * cellSize, cellSize, cellSize, 'w');
        level->addCollectable(new Ring(93 * cellSize, 8 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(94 * cellSize, 8 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new ExtraLife(95 * cellSize, 8 * cellSize, cellSize, cellSize, lifeTex));
        level->addEnemy(new BatBrain(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            97 * cellSize, 7 * cellSize));

        level->addObstacle(new Platform(120 * cellSize, 3 * cellSize, 4 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(121 * cellSize, 2 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(122 * cellSize, 2 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(123 * cellSize, 2 * cellSize, cellSize, cellSize, ringTex));
        level->addEnemy(new BeeBot(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            125 * cellSize, 4 * cellSize));
        level->addObstacle(new Spikes(130 * cellSize, 12 * cellSize, cellSize, cellSize, spikesTex));
        level->markObstacle(130 * cellSize, 12 * cellSize, cellSize, cellSize, 's');
        level->addObstacle(new Platform(140 * cellSize, 6 * cellSize, 5 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(141 * cellSize, 5 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(142 * cellSize, 5 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new SpecialBoost(143 * cellSize, 5 * cellSize, cellSize, cellSize, boostTex));
        level->addEnemy(new CrabMeat(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0right_still.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            160 * cellSize, 10 * cellSize, 160 * cellSize, 170 * cellSize));
        level->addEnemy(new Motobug(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            175 * cellSize, 10 * cellSize));
        level->addObstacle(new Platform(180 * cellSize, 2 * cellSize, 3 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(181 * cellSize, 1 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(182 * cellSize, 1 * cellSize, cellSize, cellSize, ringTex));

        levels[levelCount++] = level;
    }

    void createLevel2()
    {
        const int width = 250;
        const int height = 14;
        const int cellSize = 64;
        const int maxObstacles = 40;
        const int maxCollectables = 60;
        const int maxEnemies = 10;

        Level *level = new Level(width, height, cellSize, maxObstacles, maxCollectables, maxEnemies, "Level 2");
        level->setBackground("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/bg2.png");
        level->setWallTexture("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/brick1.png");
        level->setMusic("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/labrynth.ogg");

        for (int i = 0; i < height; ++i)
        {
            if (i == 11)
            {
                for (int j = 0; j < width; j++)
                {
                    if (j >= 80 && j <= 85)
                        level->setCell(i, j, ' ');
                    else
                        level->setCell(i, j, 'w');
                }
            }
        }
        level->addObstacle(new Platform(10 * cellSize, 10 * cellSize, 5 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(11 * cellSize, 9 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(12 * cellSize, 9 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(13 * cellSize, 9 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(14 * cellSize, 9 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new SpecialBoost(15 * cellSize, 9 * cellSize, cellSize, cellSize, boostTex));
        level->addEnemy(new CrabMeat(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0right_still.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            25 * cellSize, 10 * cellSize, 25 * cellSize, 35 * cellSize));
        level->addEnemy(new Motobug(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            40 * cellSize, 10 * cellSize));
        level->addObstacle(new Spikes(20 * cellSize, 10 * cellSize, cellSize, cellSize, spikesTex));
        level->markObstacle(20 * cellSize, 10 * cellSize, cellSize, cellSize, 's');
        level->addObstacle(new Platform(30 * cellSize, 8 * cellSize, 4 * cellSize, 1 * cellSize, platformTex));
        level->addObstacle(new Platform(35 * cellSize, 7 * cellSize, 3 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(36 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(37 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(38 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));

        level->addObstacle(new BottomlessPit(80 * cellSize, 11 * cellSize, 16 * cellSize, 1 * cellSize));
        level->markObstacle(80 * cellSize, 11 * cellSize, 16 * cellSize, 1 * cellSize, ' ');
        level->addCollectable(new Ring(81 * cellSize, 5 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(82 * cellSize, 4 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(83 * cellSize, 3 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(84 * cellSize, 2 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(85 * cellSize, 3 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(86 * cellSize, 4 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(87 * cellSize, 5 * cellSize, cellSize, cellSize, ringTex));
        level->addEnemy(new BeeBot(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            87 * cellSize, 1 * cellSize));
        level->addObstacle(new Wall(120 * cellSize, 10 * cellSize, cellSize, 2 * cellSize, wallTex1));
        level->markObstacle(120 * cellSize, 10 * cellSize, cellSize, 2 * cellSize, 'w');
        level->addObstacle(new Wall(121 * cellSize, 10 * cellSize, cellSize, 2 * cellSize, wallTex1));
        level->markObstacle(121 * cellSize, 10 * cellSize, cellSize, 2 * cellSize, 'w');
        level->addObstacle(new Wall(122 * cellSize, 10 * cellSize, cellSize, 2 * cellSize, wallTex1));
        level->markObstacle(122 * cellSize, 10 * cellSize, cellSize, 2 * cellSize, 'w');
        level->addObstacle(new Wall(123 * cellSize, 9 * cellSize, cellSize, 2 * cellSize, wallTex1));
        level->markObstacle(123 * cellSize, 9 * cellSize, cellSize, 2 * cellSize, 'w');
        level->addObstacle(new Wall(124 * cellSize, 9 * cellSize, cellSize, 2 * cellSize, wallTex1));
        level->markObstacle(124 * cellSize, 9 * cellSize, cellSize, 2 * cellSize, 'w');
        level->addObstacle(new Wall(120 * cellSize, 8 * cellSize, cellSize, cellSize, wallTex1));
        level->markObstacle(120 * cellSize, 8 * cellSize, cellSize, cellSize, 'w');
        level->addObstacle(new Wall(121 * cellSize, 8 * cellSize, cellSize, cellSize, wallTex1));
        level->markObstacle(121 * cellSize, 8 * cellSize, cellSize, cellSize, 'w');
        level->addCollectable(new Ring(125 * cellSize, 7 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(126 * cellSize, 7 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(127 * cellSize, 7 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new ExtraLife(128 * cellSize, 7 * cellSize, cellSize, cellSize, lifeTex));
        level->addEnemy(new BatBrain(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            130 * cellSize, 6 * cellSize));
        level->addObstacle(new Platform(150 * cellSize, 2 * cellSize, 4 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(151 * cellSize, 1 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(152 * cellSize, 1 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(153 * cellSize, 1 * cellSize, cellSize, cellSize, ringTex));
        level->addEnemy(new BeeBot(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            155 * cellSize, 3 * cellSize));
        level->addEnemy(new Motobug(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            165 * cellSize, 10 * cellSize));
        level->addObstacle(new Spikes(160 * cellSize, 12 * cellSize, cellSize, cellSize, spikesTex));
        level->markObstacle(160 * cellSize, 12 * cellSize, cellSize, cellSize, 's');
        level->addObstacle(new Platform(170 * cellSize, 5 * cellSize, 5 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(171 * cellSize, 4 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(172 * cellSize, 4 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new SpecialBoost(173 * cellSize, 4 * cellSize, cellSize, cellSize, boostTex));
        level->addEnemy(new CrabMeat(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0right_still.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            190 * cellSize, 10 * cellSize, 190 * cellSize, 200 * cellSize));
        level->addObstacle(new Platform(210 * cellSize, 1 * cellSize, 4 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(211 * cellSize, 0 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(212 * cellSize, 0 * cellSize, cellSize, cellSize, ringTex));
        level->addObstacle(new Spikes(220 * cellSize, 10 * cellSize, cellSize, cellSize, spikesTex));
        level->markObstacle(220 * cellSize, 10 * cellSize, cellSize, cellSize, 's');

        levels[levelCount++] = level;
    }

    void createLevel3()
    {
        const int width = 300;
        const int height = 14;
        const int cellSize = 64;
        const int maxObstacles = 40;
        const int maxCollectables = 60;
        const int maxEnemies = 10;

        Level *level = new Level(width, height, cellSize, maxObstacles, maxCollectables, maxEnemies, "Level 3");
        level->setBackground("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/level--3.png");
        level->setWallTexture("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/brick1.png");
        level->setMusic("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/labrynth.ogg");
        for (int i = 0; i < height; ++i)
        {
            if (i == 11)
            {
                for (int j = 0; j < width; j++)
                {
                    if (j >= 100 && j <= 110)
                        level->setCell(i, j, ' ');
                    else
                        level->setCell(i, j, 'w');
                }
            }
        }
        level->addObstacle(new Platform(10 * cellSize, 10 * cellSize, 5 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(11 * cellSize, 9 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(12 * cellSize, 9 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(13 * cellSize, 9 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(14 * cellSize, 9 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new SpecialBoost(15 * cellSize, 9 * cellSize, cellSize, cellSize, boostTex));
        level->addEnemy(new CrabMeat(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0right_still.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            25 * cellSize, 10 * cellSize, 25 * cellSize, 35 * cellSize));
        level->addEnemy(new Motobug(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            45 * cellSize, 10 * cellSize));
        level->addObstacle(new Spikes(20 * cellSize, 10 * cellSize, cellSize, cellSize, spikesTex));
        level->markObstacle(20 * cellSize, 10 * cellSize, cellSize, cellSize, 's');
        level->addObstacle(new Platform(30 * cellSize, 8 * cellSize, 4 * cellSize, 1 * cellSize, platformTex));
        level->addObstacle(new Platform(35 * cellSize, 7 * cellSize, 3 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(36 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(37 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(38 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));
        level->addObstacle(new Platform(50 * cellSize, 6 * cellSize, 5 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(51 * cellSize, 5 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(52 * cellSize, 5 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(53 * cellSize, 5 * cellSize, cellSize, cellSize, ringTex));

        level->addObstacle(new BottomlessPit(100 * cellSize, 11 * cellSize, 21 * cellSize, 1 * cellSize));
        level->markObstacle(100 * cellSize, 11 * cellSize, 21 * cellSize, 1 * cellSize, ' ');
        level->addCollectable(new Ring(101 * cellSize, 4 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(102 * cellSize, 3 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(103 * cellSize, 2 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(104 * cellSize, 1 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(105 * cellSize, 0 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(106 * cellSize, 1 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(107 * cellSize, 2 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(108 * cellSize, 3 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(109 * cellSize, 4 * cellSize, cellSize, cellSize, ringTex));
        level->addEnemy(new BeeBot(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            110 * cellSize, 0 * cellSize));
        level->addObstacle(new Wall(150 * cellSize, 10 * cellSize, cellSize, 3 * cellSize, wallTex1));
        level->markObstacle(150 * cellSize, 10 * cellSize, cellSize, 3 * cellSize, 'w');
        level->addObstacle(new Wall(151 * cellSize, 10 * cellSize, cellSize, 3 * cellSize, wallTex1));
        level->markObstacle(151 * cellSize, 10 * cellSize, cellSize, 3 * cellSize, 'w');
        level->addObstacle(new Wall(152 * cellSize, 10 * cellSize, cellSize, 3 * cellSize, wallTex1));
        level->markObstacle(152 * cellSize, 10 * cellSize, cellSize, 3 * cellSize, 'w');
        level->addObstacle(new Wall(153 * cellSize, 9 * cellSize, cellSize, 3 * cellSize, wallTex1));
        level->markObstacle(153 * cellSize, 9 * cellSize, cellSize, 3 * cellSize, 'w');
        level->addObstacle(new Wall(154 * cellSize, 9 * cellSize, cellSize, 3 * cellSize, wallTex1));
        level->markObstacle(154 * cellSize, 9 * cellSize, cellSize, 3 * cellSize, 'w');
        level->addObstacle(new Wall(150 * cellSize, 8 * cellSize, cellSize, 2 * cellSize, wallTex1));
        level->markObstacle(150 * cellSize, 8 * cellSize, cellSize, 2 * cellSize, 'w');
        level->addObstacle(new Wall(151 * cellSize, 8 * cellSize, cellSize, 2 * cellSize, wallTex1));
        level->markObstacle(151 * cellSize, 8 * cellSize, cellSize, 2 * cellSize, 'w');
        level->addObstacle(new Wall(152 * cellSize, 7 * cellSize, cellSize, 2 * cellSize, wallTex1));
        level->markObstacle(152 * cellSize, 7 * cellSize, cellSize, 2 * cellSize, 'w');
        level->addObstacle(new Wall(153 * cellSize, 7 * cellSize, cellSize, 2 * cellSize, wallTex1));
        level->markObstacle(153 * cellSize, 7 * cellSize, cellSize, 2 * cellSize, 'w');
        level->addCollectable(new Ring(155 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(156 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(157 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(158 * cellSize, 6 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new ExtraLife(159 * cellSize, 6 * cellSize, cellSize, cellSize, lifeTex));
        level->addEnemy(new BatBrain(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            160 * cellSize, 5 * cellSize));

        // Additional Elements (cols 170-299, rows 0-13)
        level->addObstacle(new Platform(180 * cellSize, 2 * cellSize, 5 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(181 * cellSize, 1 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(182 * cellSize, 1 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(183 * cellSize, 1 * cellSize, cellSize, cellSize, ringTex));
        level->addEnemy(new BeeBot(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            185 * cellSize, 3 * cellSize));
        level->addEnemy(new Motobug(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            195 * cellSize, 10 * cellSize)); // Added Motobug in additional elements
        level->addObstacle(new Spikes(190 * cellSize, 12 * cellSize, cellSize, cellSize, spikesTex));
        level->markObstacle(190 * cellSize, 12 * cellSize, cellSize, cellSize, 's');
        level->addObstacle(new Platform(200 * cellSize, 5 * cellSize, 5 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(201 * cellSize, 4 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(202 * cellSize, 4 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new SpecialBoost(203 * cellSize, 4 * cellSize, cellSize, cellSize, boostTex));
        level->addEnemy(new CrabMeat(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0right_still.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            220 * cellSize, 10 * cellSize, 220 * cellSize, 230 * cellSize));
        level->addObstacle(new Platform(240 * cellSize, 1 * cellSize, 4 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(241 * cellSize, 0 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(242 * cellSize, 0 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(243 * cellSize, 0 * cellSize, cellSize, cellSize, ringTex));
        level->addObstacle(new Spikes(250 * cellSize, 10 * cellSize, cellSize, cellSize, spikesTex));
        level->markObstacle(250 * cellSize, 10 * cellSize, cellSize, cellSize, 's');
        level->addObstacle(new Platform(270 * cellSize, 3 * cellSize, 5 * cellSize, 1 * cellSize, platformTex));
        level->addCollectable(new Ring(271 * cellSize, 2 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(272 * cellSize, 2 * cellSize, cellSize, cellSize, ringTex));
        level->addCollectable(new Ring(273 * cellSize, 2 * cellSize, cellSize, cellSize, ringTex));
        level->addEnemy(new BeeBot(
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/0hit_box.png",
            "C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/ring.png",
            275 * cellSize, 4 * cellSize));

        levels[levelCount++] = level;
    }

    void createBossLevel()
    {
        const int width = 40;
        const int height = 14;
        const int cellSize = 64;
        const int maxObstacles = 10;
        const int maxCollectables = 0;
        const int maxEnemies = 1;

        Level *level = new Level(width, height, cellSize, maxObstacles, maxCollectables, maxEnemies, "Boss Level");
        level->setBackground("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/bg2.png");
        level->setWallTexture("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/brick1.png");
        level->setMusic("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/labrynth.ogg");
        for (int j = 0; j < width; j++)
        {
            level->setCell(11, j, 'w');
        }
        level->addEnemy(new EggStinger("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/0hit_box.png", 5.0f, 2.5f));

        levels[levelCount++] = level;
    }
};
class PlayerManager
{
private:
    player **players;
    int currentPlayerIndex;
    int playerCount;
    float followDistance;

public:
    PlayerManager(PlayerFactory &factory)
        : currentPlayerIndex(0), followDistance(64.0f)
    {
        playerCount = factory.getSize();
        players = new player *[playerCount];
        for (int i = 0; i < playerCount; i++)
        {
            players[i] = factory.getPlayer(i);
        }
    }

    ~PlayerManager()
    {
        delete[] players;
    }

    player *getCurrentPlayer()
    {
        return players[currentPlayerIndex];
    }

    void switchPlayer()
    {
        currentPlayerIndex = (currentPlayerIndex + 1) % playerCount;
    }

    void updateFollowers()
    {
        float mainX = players[currentPlayerIndex]->getX();
        float mainY = players[currentPlayerIndex]->getY();
        int followIndex = 1;
        for (int i = 0; i < playerCount; i++)
        {
            if (i != currentPlayerIndex)
            {
                float targetX = mainX - followIndex * followDistance;
                players[i]->setPosition(targetX, mainY);
                followIndex++;
            }
        }
    }

    void drawAll(RenderWindow &window, float scrollX)
    {
        for (int i = 0; i < playerCount; i++)
        {
            if (i != currentPlayerIndex)
            {
                players[i]->draw(window, scrollX);
            }
        }
        players[currentPlayerIndex]->draw(window, scrollX);
    }
};
class Animator
{
protected:
    player *tails;
    player *sonic;
    player *knuckles;
    bool isActive;

public:
    Animator(player *tailsPtr, player *sonicPtr, player *knucklesPtr)
        : tails(tailsPtr), sonic(sonicPtr), knuckles(knucklesPtr), isActive(false)
    {
    }

    virtual ~Animator() {}

    virtual void update() = 0;
    virtual void draw(RenderWindow &window, float scrollX) = 0;

    bool isActiveState() const { return isActive; }
};
class TailsFly : public Animator
{
private:
    Texture tailsFlyingTexture;
    Texture sonicTexture;
    Texture knucklesTexture;

    Sprite tailsFlyingSprite;
    Sprite sonicSprite;
    Sprite knucklesSprite;

    float verticalSpacing = 80.0f;
    float movementSpeed = 150.0f;

public:
    TailsFly(player *tailsPtr, player *sonicPtr, player *knucklesPtr)
        : Animator(tailsPtr, sonicPtr, knucklesPtr)
    {
        tailsFlyingTexture.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/tails-f.png");
        sonicTexture.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/sonic_fly.jpg");
        knucklesTexture.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/knuckles-hang.png");
        tailsFlyingSprite.setTexture(tailsFlyingTexture);
        sonicSprite.setTexture(sonicTexture);
        knucklesSprite.setTexture(knucklesTexture);
        float scale = 2.5f;
        tailsFlyingSprite.setScale(scale, scale);
        sonicSprite.setScale(scale, scale);
        knucklesSprite.setScale(scale, scale);
    }

    void update() override
    {
        Tails *tailsObj = dynamic_cast<Tails *>(tails);

        if (tailsObj->isFly())
        {
            isActive = true;
            float currentX = tails->getX();
            float newX = currentX + movementSpeed * (1.0f / 60.0f);
            tails->setPosition(newX, 50.0f);
            sonic->setPosition(newX, 50.0f + verticalSpacing);
            knuckles->setPosition(newX, 70.0f + verticalSpacing * 2);
            tails->setVelocityY(0);
            sonic->setVelocityY(0);
            knuckles->setVelocityY(0);
            tails->setOnGround(false);
            sonic->setOnGround(false);
            knuckles->setOnGround(false);
        }
        else
        {
            if (isActive)
            {
                sonic->setVelocityY(2.0f);
                knuckles->setVelocityY(2.0f);
            }
            isActive = false;
        }
    }
    void draw(RenderWindow &window, float scrollX) override
    {
        if (!isActive)
            return;
        knucklesSprite.setPosition(knuckles->getX() - scrollX, knuckles->getY());
        window.draw(knucklesSprite);
        sonicSprite.setPosition(sonic->getX() - scrollX, sonic->getY());
        window.draw(sonicSprite);
        tailsFlyingSprite.setPosition(tails->getX() - scrollX, tails->getY());
        window.draw(tailsFlyingSprite);
    }
};
class tim
{
private:
    sf::Clock clock;
    sf::Time elapsedTime;
    sf::Font font;
    sf::Text timeText;
    bool isRunning;

public:
    tim() : isRunning(false)
    {
        if (!font.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/NRT-Reg.ttf"))
        {
        }
        timeText.setFont(font);
        timeText.setCharacterSize(24);
        timeText.setFillColor(sf::Color::White);
        timeText.setPosition(10, 10);
    }

    void start()
    {
        if (!isRunning)
        {
            clock.restart();
            isRunning = true;
        }
    }

    void stop()
    {
        if (isRunning)
        {
            elapsedTime += clock.getElapsedTime();
            isRunning = false;
        }
    }

    void reset()
    {
        elapsedTime = sf::Time::Zero;
        if (isRunning)
        {
            clock.restart();
        }
    }

    void update()
    {
        if (isRunning)
        {
            sf::Time currentTime = elapsedTime + clock.getElapsedTime();
            int seconds = static_cast<int>(currentTime.asSeconds());
            int minutes = seconds / 60;
            seconds %= 60;

            string timeString = string("Time: ") +
                                (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
                                (seconds < 10 ? "0" : "") + std::to_string(seconds);
            timeText.setString(timeString);
        }
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(timeText);
    }
};

class Score
{
private:
    int score;
    int rings;
    sf::Font font;
    sf::Text scoreText;
    sf::Text ringsText;

public:
    Score() : score(0), rings(0)
    {
        if (!font.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/NRT-Reg.ttf"))
        {
        }

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 40);

        ringsText.setFont(font);
        ringsText.setCharacterSize(24);
        ringsText.setFillColor(sf::Color::White);
        ringsText.setPosition(10, 70);

        updateText();
    }

    void addRing()
    {
        rings++;
        score += 10;
        updateText();
    }

    void reset()
    {
        score = 0;
        rings = 0;
        updateText();
    }

    int getScore() const { return score; }
    int getRings() const { return rings; }

    void updateText()
    {
        scoreText.setString("Score: " + std::to_string(score));
        ringsText.setString("Rings: " + std::to_string(rings));
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(scoreText);
        window.draw(ringsText);
    }
};
class GameOverMenu
{
private:
    static const int MAX_ITEMS = 2;
    sf::Font font;
    sf::Text gameOverText;
    sf::Text menuItems[MAX_ITEMS];
    int selectedIndex;
    sf::Color normalColor;
    sf::Color selectedColor;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

public:
    GameOverMenu(float width, float height)
        : selectedIndex(0),
          normalColor(sf::Color::White),
          selectedColor(sf::Color::Yellow)
    {

        if (!font.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/NRT-Reg.ttf"))
        {
            std::cerr << "Failed to load font for GameOverMenu!" << std::endl;
        }
        if (!backgroundTexture.loadFromFile("C:/Users/talha/source/repos/Project-oop/Project-oop/x64/Debug/Data/bg2.png"))
        {
            std::cerr << "Failed to load game over background texture!" << std::endl;
            backgroundSprite.setTextureRect(sf::IntRect(0, 0, 1, 1));
            backgroundSprite.setColor(sf::Color(50, 50, 100));
            backgroundSprite.setScale(width, height);
        }
        else
        {
            backgroundSprite.setTexture(backgroundTexture);
            backgroundSprite.setScale(width / backgroundTexture.getSize().x, height / backgroundTexture.getSize().y);
        }
        gameOverText.setFont(font);
        gameOverText.setString("Game Over");
        gameOverText.setCharacterSize(72);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setPosition(width / 2.0f - 150.0f, height / 4.0f);
        const char *options[MAX_ITEMS] = {"Restart Level", "Main Menu"};
        float verticalSpacing = 80.0f;
        float startY = height / 2.0f;

        for (int i = 0; i < MAX_ITEMS; ++i)
        {
            menuItems[i].setFont(font);
            menuItems[i].setString(options[i]);
            menuItems[i].setCharacterSize(48);
            menuItems[i].setFillColor(i == 0 ? selectedColor : normalColor);
            menuItems[i].setPosition(width / 2.0f - 100.0f, startY + i * verticalSpacing);
        }
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(backgroundSprite);
        window.draw(gameOverText);
        for (int i = 0; i < MAX_ITEMS; ++i)
        {
            window.draw(menuItems[i]);
        }
    }

    void moveUp()
    {
        if (selectedIndex > 0)
        {
            menuItems[selectedIndex].setFillColor(normalColor);
            selectedIndex--;
            menuItems[selectedIndex].setFillColor(selectedColor);
        }
    }

    void moveDown()
    {
        if (selectedIndex < MAX_ITEMS - 1)
        {
            menuItems[selectedIndex].setFillColor(normalColor);
            selectedIndex++;
            menuItems[selectedIndex].setFillColor(selectedColor);
        }
    }

    int getSelectedIndex() const
    {
        return selectedIndex;
    }
};
class Game
{
private:
    bool gameOver;
    GameOverMenu *gameOverMenu;
    int currentLevelIndex;

public:
    static void setGameOver()
    {
        getInstance()->gameOver = true;
        getInstance()->currentLevel->stopMusic();
        player::decreaseHP();
    }

private:
    static Game *instance;
    Level *currentLevel;
    PlayerManager *playerManager;
    LevelFactory levelFactory;
    PlayerFactory playerFactory;
    Menu *menu;
    RenderWindow *window;
    bool gameRunning;
    tim gameTime;
    Score gameScore;

    Game() : currentLevel(nullptr), playerManager(nullptr), menu(nullptr), window(nullptr), gameRunning(false), gameOver(false), gameOverMenu(nullptr), currentLevelIndex(0) {}

public:
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    static Game *getInstance()
    {
        if (!instance)
        {
            instance = new Game();
        }
        return instance;
    }

    ~Game()
    {
        cleanup();
    }

    void initialize()
    {
        window = new RenderWindow(VideoMode(screen_x, screen_y), "Sonic the Hedgehog-OOP", Style::Close);
        window->setVerticalSyncEnabled(true);
        window->setFramerateLimit(60);

        playerFactory.initializeDefaults(2.5f, 2.5f);
        playerManager = new PlayerManager(playerFactory);

        menu = new Menu(screen_x, screen_y);
        gameOverMenu = new GameOverMenu(screen_x, screen_y);
        gameRunning = true;
    }

    void cleanup()
    {
        if (currentLevel)
        {
            currentLevel->stopMusic();
            delete currentLevel;
        }
        delete playerManager;
        delete menu;
        delete gameOverMenu;
        if (window)
            window->close();
        delete window;
    }
    void runGameOverMenu(int currentLevelIndex)
    {
        bool selectionMade = false;
        int selectedOption = -1;

        while (!selectionMade && window->isOpen())
        {
            Event event;
            while (window->pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window->close();

                if (event.type == Event::KeyPressed)
                {
                    if (event.key.code == Keyboard::Up)
                        gameOverMenu->moveUp();
                    else if (event.key.code == Keyboard::Down)
                        gameOverMenu->moveDown();
                    else if (event.key.code == Keyboard::Enter)
                    {
                        selectedOption = gameOverMenu->getSelectedIndex();
                        selectionMade = true;
                    }
                    else if (event.key.code == Keyboard::Escape)
                        window->close();
                }
            }

            window->clear();
            gameOverMenu->draw(*window);
            window->display();
        }

        if (window->isOpen())
        {
            if (selectedOption == 0)
            {
                gameOver = false;
                gameScore.reset();
                player::increaseHP(3);
                startLevel(currentLevelIndex);
            }
            else if (selectedOption == 1)
            {
                gameRunning = false;
                gameOver = false;
                gameScore.reset();
                player::increaseHP(3);
                runMainMenu();
            }
        }
    }
    void runMainMenu()
    {
        bool levelSelected = false;
        int selectedLevel = -1;

        while (!levelSelected && window->isOpen())
        {
            Event event;
            while (window->pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window->close();

                if (event.type == Event::KeyPressed)
                {
                    if (event.key.code == Keyboard::Up)
                        menu->moveUp();
                    else if (event.key.code == Keyboard::Down)
                        menu->moveDown();
                    else if (event.key.code == Keyboard::Enter)
                    {
                        selectedLevel = menu->getSelectedIndex();
                        levelSelected = true;
                    }
                    else if (event.key.code == Keyboard::Escape)
                        window->close();
                }
            }

            window->clear();
            menu->draw(*window);
            window->display();
        }

        if (window->isOpen())
        {
            startLevel(selectedLevel);
        }
    }

    void startLevel(int levelIndex)
    {
        if (currentLevel)
        {
            currentLevel->stopMusic();
            delete currentLevel;
        }

        currentLevel = levelFactory.getLevel(levelIndex);
        currentLevelIndex = levelIndex;
        if (!currentLevel)
            return;
        gameTime.start();
        currentLevel->playMusic();
        runGameLoop();
    }

    void runGameLoop()
    {
        TailsFly tailsFlyAnimator(
            playerFactory.getPlayer(1),
            playerFactory.getPlayer(0),
            playerFactory.getPlayer(2));

        const float maxScrollX = currentLevel->getWidth() * currentLevel->getCellSize() - screen_x;

        while (window->isOpen() && gameRunning && !gameOver)
        {
            Event ev;
            while (window->pollEvent(ev))
            {
                if (ev.type == Event::Closed)
                    window->close();

                if (ev.type == Event::KeyPressed)
                {
                    if (ev.key.code == Keyboard::Z)
                    {
                        playerManager->switchPlayer();
                    }
                    else if (ev.key.code == Keyboard::Escape)
                    {
                        gameRunning = false;
                        runMainMenu();
                        return;
                    }
                }
            }

            player *currentPlayer = playerManager->getCurrentPlayer();
            int mapWidth = currentLevel->getWidth() * currentLevel->getCellSize();
            int mapHeight = currentLevel->getHeight() * currentLevel->getCellSize();
            currentPlayer->clampPosition(mapWidth, mapHeight);
            currentPlayer->setGravityBasedOnLevel(currentLevel->getName());
            currentPlayer->setAccelerationBasedOnLevel(currentLevel->getName());
            currentPlayer->controller();
            currentPlayer->player_gravity(currentLevel->getLevelData(), currentLevel->getCellSize());
            currentPlayer->update();
            Knuckles *knuckles = dynamic_cast<Knuckles *>(currentPlayer);
            if (knuckles)
            {
                knuckles->breakWalls(currentLevel, currentLevel->getObstacles(), currentLevel->getObstacleCount(), 20.f);
            }

            playerManager->updateFollowers();
            for (int i = 0; i < currentLevel->getCollectableCount(); ++i)
            {
                Collectable *col = currentLevel->getCollectables()[i];
                if (dynamic_cast<Ring *>(col))
                {
                    if (col->checkCollision(currentPlayer))
                    {
                        gameScore.addRing();
                    }
                }
            }

            currentLevel->checkCollisions(currentPlayer);
            currentLevel->update(currentPlayer, 1.f / 60.f);

            float playerMiddle = currentPlayer->getX() + currentPlayer->getWidth() / 2;
            float scrollX = playerMiddle - screen_x / 2;
            if (scrollX < 0)
                scrollX = 0;
            if (scrollX > maxScrollX)
                scrollX = maxScrollX;

            window->clear();
            currentLevel->draw(*window, scrollX);

            tailsFlyAnimator.update();
            if (tailsFlyAnimator.isActiveState())
            {
                tailsFlyAnimator.draw(*window, scrollX);
            }
            else
            {
                playerManager->drawAll(*window, scrollX);
            }
            gameTime.update();
            gameTime.draw(*window);
            gameScore.draw(*window);

            window->display();
        }
        if (gameOver)
        {
            runGameOverMenu(currentLevelIndex);
        }
    }
};

Game *Game::instance = nullptr;
void BottomlessPit::check(player &p)
{
    float px = p.getX();
    float py = p.getY();
    float pw = p.getWidth();
    float ph = p.getHeight();
    bool horizontallyOverPit = (px + pw > x) && (px < x + width);
    bool fallenThrough = py + ph > y + height;
    if (horizontallyOverPit && fallenThrough)
        Game::setGameOver();
}

int main()
{
    Game *game = Game::getInstance();
    game->initialize();
    game->runMainMenu();
    return 0;
}
void draw_player(RenderWindow &window, Sprite &LstillSprite, float player_x, float player_y)
{
    LstillSprite.setPosition(player_x, player_y);
    window.draw(LstillSprite);
}
void display_level(RenderWindow &window, const int height, const int width, char **lvl, Sprite &wallSprite1, const int cell_size, float scrollX)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (lvl[i][j] == 'w')
            {
                wallSprite1.setPosition(j * cell_size - scrollX, i * cell_size);
                window.draw(wallSprite1);
            }
        }
    }
}
