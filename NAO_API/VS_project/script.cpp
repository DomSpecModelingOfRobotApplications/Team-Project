#include "apidemonstration.h"

void APIDemonstration::script()
{
    //show_postures();
    //move_navigation(1, 0, 0);
    //take_picture("lol");
    //say_phrase("Here we go!", "English");
    //say_phrase("Bonjour! Un, deux, trois, Je ne sais pas", "French");
    //disagree();
    //agree();
    //posture("Sit");
    //rest();
    //wake_up();
    //subscribe_to_event();
    //get_visual();
    //not_these_droids();
    //bow();
    //face_detection();
    //bool face = is_it_a_face(5);
    //if (face)
    //    say_phrase("A face is detected", "English");
    //else 
    //    say_phrase("It is not a face", "English");
    //darkness_detection();
    //darkness_detection();
    //disagree();
    //agree();
    //stop_face_detection();
    //stop_darkness_detection();
    //if (detect_red_ball(5))
    //    std::cout << "Yepp" << std::endl;
    //else
    //    std::cout << "Nope" << std::endl;
    if (detect_obstacle(3))
        std::cout << "Yepp" << std::endl;
    else
        std::cout << "Nope" << std::endl;
}