//
// Created by brabect on 04.10.26.
//

#ifndef POSITIONBASEDFLUIDS_UI_H
#define POSITIONBASEDFLUIDS_UI_H
#include <functional>


class UI {
public:
    UI(std::function<void()> render);
    UI();
    std::function<void()> Render;
private:
};


#endif //POSITIONBASEDFLUIDS_UI_H
