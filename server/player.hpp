#pragma once

#include "config.hpp"
#include "entity.hpp"
#include "log.hpp"

namespace autocraft {

class connection;

class AUTOCRAFT_API player : public mobile_entity
{
    AUTOCRAFT_NON_COPYABLE(player);

private:
    connection *player_conn_;
    vector target_pos_;
    bool moving_to_;

public:
    player();

public:
    virtual bool           update(double d, env & env) override;
    virtual nlohmann::json save() const override;
    virtual void           load(nlohmann::json const& j) override;

    void move_to(vector const& target_pos);
    void stop();
    bool is_moving_to() const;

    void         set_conn(connection *conn);
    connection *conn();

#ifdef AUTOCRAFT_TESTS
public:
    virtual bool operator==(entity const& other) const override;
    virtual bool operator!=(entity const& other) const override;
#endif /* AUTOCRAFT_TESTS */
};

} // namespace autocraft
