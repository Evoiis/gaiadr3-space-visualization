#ifndef STARDATA_H
#define STARDATA_H

#include <string>
#include <vector>
#include <mutex>
#include <memory>

#include <glm/glm.hpp>


// std::map<int64_t, int> star_id_to_index;
struct StarData {    
    std::vector<glm::vec3> star_positions; // xyz, Parsecs
    
    std::vector<glm::vec3> star_colors_rgb;
    std::vector<float> star_brightness;
    std::vector<float> star_sizes;

    std::vector<std::string> star_names;
};


using StarDataPtr = std::shared_ptr<StarData>;
class SharedStars{
public:

    StarDataPtr try_get_ptr(){
        if (m_data_lock.try_lock()) {
            std::lock_guard<std::mutex> lock(m_data_lock, std::adopt_lock);
            return m_stars;
            m_updated = false;
        }
        return nullptr;
    }
    
    bool check_if_updated(){
        std::lock_guard<std::mutex> lock(m_data_lock);
        return m_updated;
    }

    void set(StarDataPtr new_ptr){
        std::lock_guard<std::mutex> lock(m_data_lock);
        m_stars = new_ptr;
        m_updated = true;
    }

private:
    StarDataPtr m_stars = nullptr;
    std::mutex m_data_lock;
    bool m_updated = false;

};


#endif
