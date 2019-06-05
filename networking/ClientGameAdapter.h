#pragma once

#include "yojimbo.h"
#include "GameMessageFactory.h"

class ClientGameAdapter : public yojimbo::Adapter {
public:
    explicit ClientGameAdapter() {}

    yojimbo::MessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override {
        return YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
    }
    
private:
};
