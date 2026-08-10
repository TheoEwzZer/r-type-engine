/*
** EPITECH PROJECT, 2024
** rtype
** File description:
** main
*/

#include "src/ClientEngine/ClientEngine.hpp"
#include "src/EditorEngine/EditorEngine.hpp"
#include "src/MainMenu/MainMenu.hpp"
#include "src/Utils/ArgsParser/ArgsParser.hpp"
#include <iostream>
#include <memory>

int main(int argc, char **argv)
{
    ArgsParser parser;
    bool spectatorMode = false;

    try {
        parser.parseArgs(argc, argv);
        int skin = parser.getSkin();
        for (int i = 1; i < argc; ++i) {
            if (string(argv[i]) == "-s" || string(argv[i]) == "--spectator") {
                spectatorMode = true;
                break;
            }
        }
        auto engine = make_unique<ClientEngine>(argv[1] ? argv[1] : "");
        engine->getWindow().create(sf::VideoMode(1128, 672), "R-Type");
        engine->getWindow().setFramerateLimit(60);
        auto client = make_unique<Client>(engine->getWindow(),
            engine->getInputManager(), spectatorMode, skin);

        shared_ptr<EditorEngine> editor
            = make_shared<EditorEngine>(engine->getWindow());

        auto menu = make_unique<MainMenu>(*engine, *client, editor);

        while (engine->getWindow().isOpen()) {
            bool goEditor = menu->menuLoop(engine->getWindow());
            if (goEditor) {
                editor->run();
            } else {
                client->run();
            }
        }
    } catch (const HelpException &e) {
        cerr << e.what() << endl;
        return 0;
    } catch (const InvalidArgumentException &e) {
        cerr << e.what() << endl;
        return 84;
    }
}
