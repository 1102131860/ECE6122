/*
Author: Rui Wang
Class: ECE6122
Last Date Modified: Sep 26 2024
Description:
ECE Centipede source file
*/

#include "ECE_Centipede.h"
#include <iostream>

// Define the static texture members
sf::Texture ECE_Centipede::centipedeHeadTexture;
sf::Texture ECE_Centipede::centipedeBodyTexture;

// Constructor for Segment
ECE_Centipede::Segment::Segment(sf::Texture& texture, float x, float y){
    setTexture(texture);
    setPosition(x, y);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//      There may be a bug here for the collision between mushroom and boundary            //
/////////////////////////////////////////////////////////////////////////////////////////////

// Move the segment based on direction and whether it's moving up/down
void ECE_Centipede::Segment::moveWithCheckingMushroomCollision(float speed, float deltaTime, bool collideWithMushroom) {
    // Check if the segment collides with a mushroom or left or right boundary
    if (collideWithMushroom || getPosition().x <= 0 || getPosition().x + getGlobalBounds().width >= WINDOW_WIDTH) {
        // If the segment touches the top or bottom of the window, invert the vertical moving direction
        if (getPosition().y + getGlobalBounds().height >= WINDOW_HEIGHT || getPosition().y <= INFO_AREA_HEIGHT) {
            isMovingUp = !isMovingUp;  // Invert vertical direction
        }

        // Move vertically by the height of the sprite, also need move horizontally back by one width
        move(direction.x < 0 ? 1.f : -1.f,
            isMovingUp ? static_cast<float>(getGlobalBounds().height) : static_cast<float>(-getGlobalBounds().height));

        // Invert the horizontal moving direction after vertical move
        direction.x = -direction.x;

        // Rotate the texture
        setOrigin(getGlobalBounds().width / 2, getGlobalBounds().height / 2);
        setRotation(direction.x > 0 ? 0.f : 180.f);
    }
    else {
        // If no collision, move horizontally
        move(direction.x * speed * deltaTime, 0);
    }
}

// Check if intersect with other spirt
bool ECE_Centipede::Segment::checkCollisionWithOtherSprit(sf::Sprite thatSpirt) {
    return getGlobalBounds().intersects(thatSpirt.getGlobalBounds());
}

// Constructor for ECE_Centipede
ECE_Centipede::ECE_Centipede(int numSegments, float startX, float startY) {
    // Create the head
    Segment headSegment(centipedeHeadTexture, startX, startY);
    // Rescale as centipedeBody size
    float headScaleX = static_cast<float>(centipedeBodyTexture.getSize().x) / centipedeHeadTexture.getSize().x;
    float headScaleY = static_cast<float>(centipedeBodyTexture.getSize().y) / centipedeHeadTexture.getSize().y;
    headSegment.setScale(headScaleX, headScaleY);
    // Set the origin to the center of the texture before rotating
    setOrigin(getGlobalBounds().width / 2, getGlobalBounds().height / 2);
    // Rotate 180 degree 
    headSegment.setRotation(180.f);
    // Set the first element as head
    segments.push_back(headSegment);

    // Create the body segments
    for (int i = 1; i < numSegments; ++i) {
        float bodyX = startX + i * headSegment.getGlobalBounds().width;
        Segment bodySegment(centipedeBodyTexture, bodyX, startY);
        // Set the origin to the center of the texture before rotating
        setOrigin(getGlobalBounds().width / 2, getGlobalBounds().height / 2);
        // Rotate 180 degree
        bodySegment.setRotation(180.f);  // Moving to the left
        segments.push_back(bodySegment);
    }
}

// Update centipede movement with checking collision with mushrooms
void ECE_Centipede::moveWithCheckingCollisionWithMushrooms(float deltaTime, std::list<ECE_Mushroom>& mushrooms) {
    // Move head by checking collision with mushroom or not
    Segment headSegment = segments.front();
    bool headSegmentCollidesWithMushroom = false;
    for (auto& mushroom : mushrooms) {
        if (headSegment.checkCollisionWithOtherSprit(mushroom)) {
            headSegmentCollidesWithMushroom = true;
            break;
        }
    }

    // After checking with mushroom, also check the boundary
    segments.front().moveWithCheckingMushroomCollision(speed, deltaTime, headSegmentCollidesWithMushroom);  // not headSegment

    // Move body accoding to the movement of head Segment
    for (auto bodySegmentIt = std::next(segments.begin()); bodySegmentIt != segments.end(); ++bodySegmentIt) {
        sf::Vector2f moveDirection = std::prev(bodySegmentIt)->getPosition() - bodySegmentIt->getPosition();
        float moveDistance = distance(std::prev(bodySegmentIt)->getPosition(), bodySegmentIt->getPosition());   // if there is a least Following distance

        // Move
        if (moveDistance > FOLLOW_DISTANCE_THRESHOLD) {                                             // This threshold should be tuning
            bodySegmentIt->move(normalize(moveDirection) * (speed * deltaTime));
        }

        // Rotate the texture
        if (moveDirection.x * bodySegmentIt->direction.x < 0) {
            bodySegmentIt->setOrigin(bodySegmentIt->getGlobalBounds().width / 2, bodySegmentIt->getGlobalBounds().height / 2);
            bodySegmentIt->setRotation(bodySegmentIt->direction.x > 0 ? 0.f : 180.f);
        }
    }
}

// Method to check collisions with laser blasts (if true, remove laserBlast from its list, and get score)
CentipedeHitStatus ECE_Centipede::checkCollisionWithLaserBlasts(std::list<ECE_LaserBlast>& laserBlasts, std::list<ECE_Mushroom>& mushrooms, std::list<ECE_Centipede>& centipedes) {
    CentipedeHitStatus hit = CentipedeHitStatus::NotHit;
    for (auto laserBlastIt = laserBlasts.begin(); laserBlastIt != laserBlasts.end();) {
        for (auto segmentIt = segments.begin(); segmentIt != segments.end();) {
            if (laserBlastIt->checkCollisionWithOtherSprite(*segmentIt)) {
                // Calculate the hit position
                hit = segmentIt == segments.begin() ? CentipedeHitStatus::HitHead : CentipedeHitStatus::HitBody;

                // Split the centipede
                splitCentipede(centipedes, segmentIt);

                // Create a new mushroom at the hit point
                mushrooms.emplace_back(segmentIt->getPosition().x, segmentIt->getPosition().y);

                // Remove the hit segment
                segmentIt = segments.erase(segmentIt);
                break;
            }
            else {
                ++segmentIt;
            }
        }
        if (hit != CentipedeHitStatus::NotHit) {
            laserBlastIt = laserBlasts.erase(laserBlastIt);
        }
        else {
            ++laserBlastIt;
        }
    }
    return hit;
}

// Method to check collisions with spaceship (if true, a new round or game end)
bool ECE_Centipede::checkCollisionWithSpaceship(ECE_Spaceship& spaceship) {
    for (auto& segment : segments) {
        if (segment.checkCollisionWithOtherSprit(spaceship)) {
            spaceship.hurt();
            return true;
        }
    }
    return false;
}

// Function to calculate distance between two points
float ECE_Centipede::distance(sf::Vector2f a, sf::Vector2f b) {
    return std::sqrt((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y));
}

// Function to normalize a vector
sf::Vector2f ECE_Centipede::normalize(sf::Vector2f v) {
    float mag = std::sqrt(v.x * v.x + v.y + v.y);
    if (mag == 0) return sf::Vector2f(0, 0);
    return sf::Vector2f(v.x / mag, v.y / mag);
}

// Check centipede is dead or not
bool ECE_Centipede::isDead() const {
    return segments.size() == 0;
}

// Draw the centipede
void ECE_Centipede::draw(sf::RenderWindow& window) const {
    for (const auto& segment : segments)
        window.draw(segment);
}

// Split the centipede into two at the hit segment
void ECE_Centipede::splitCentipede(std::list<ECE_Centipede>& centipedes, std::list<Segment>::iterator hitSegment) {
    std::list<Segment> rightPart;

    auto nextSegment = std::next(hitSegment);
    while (nextSegment != segments.end()) {
        rightPart.push_front(*nextSegment);              // copy the right part one by one, here push front to invert the direction
        nextSegment = segments.erase(nextSegment);      // erase right part from segments
    }

    if (!rightPart.empty()) {
        ECE_Centipede newCentipede;

        // set the bodyTexture into headTexture firstly
        rightPart.front().setTexture(centipedeHeadTexture);
        // Rescale as centipedeBody size
        float headScaleX = static_cast<float>(centipedeBodyTexture.getSize().x) / centipedeHeadTexture.getSize().x;
        float headScaleY = static_cast<float>(centipedeBodyTexture.getSize().y) / centipedeHeadTexture.getSize().y;
        rightPart.front().setScale(headScaleX, headScaleY);

        for (auto& segment : rightPart) {
            // inverse the moving direction
            segment.direction.x *= -1;
            // rotate the texture
            segment.setOrigin(segment.getGlobalBounds().width / 2, segment.getGlobalBounds().height / 2);
            segment.setRotation(segment.direction.x > 0 ? 0.f : 180.f);
        }

        // Move the right part segments into the new centipede without copying
        newCentipede.segments = std::move(rightPart);
        centipedes.push_back(newCentipede);
    }
}
