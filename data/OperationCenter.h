#ifndef OPERATIONCENTER_H_INCLUDED
#define OPERATIONCENTER_H_INCLUDED

/**
 * @brief Central controller for all game object updates & drawing.
 * @details Handles player-monster interactions and monster updates.
 */
class OperationCenter
{
public:
    static OperationCenter* get_instance() {
        static OperationCenter OC;
        return &OC;
    }

    /**
     * @brief Global update entry.
     * @details Updates all monsters and handles monster-player interactions.
     */
    void update();

    /**
     * @brief Global draw entry.
     * @details Draws all monsters.
     */
    void draw();

private:
    OperationCenter() {}

    // monster update & interaction
    void _update_monster();
    void _update_monster_player();

    // drawing
    void _draw_monster();
};

#endif