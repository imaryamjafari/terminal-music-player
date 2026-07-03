#pragma once

class Screen {
public:
    virtual ~Screen() = default; 
    virtual void render() = 0;
    virtual bool handleInput() = 0;
};
