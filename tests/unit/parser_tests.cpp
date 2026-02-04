#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

#include "../../src/io/parser/parser.hpp"
#include "../../src/events/factory/event_factory.hpp"    


class TempFile {
private:
    std::string _path;
public:
    TempFile(const std::string& content, const std::string& filename = "temp_test.txt") 
        : _path(filename) {
        std::ofstream f(_path);
        f << content;
        f.close();
    }
    ~TempFile() {
        std::filesystem::remove(_path);
    }
    const std::string& path() const { return _path; }
};

TEST(ParserTest, ValidMinimalInput) {
    TempFile tf(R"(2
09:00 18:00
50
10:00 1 client1
10:30 2 client1 1
)");
    
    std::vector<std::unique_ptr<Event>> events;
    auto club = Parser::parse_file(tf.path(), events);
    
    EXPECT_EQ(club.get_places().size(), 2);
    EXPECT_EQ(events.size(), 2);
}

TEST(ParserTest, InvalidTimeFormat) {
    TempFile tf(R"(2
09:00 18:00
50
09:65 1 client1
)");
    
    std::vector<std::unique_ptr<Event>> events;
    EXPECT_THROW({
        Parser::parse_file(tf.path(), events);
    }, std::invalid_argument);
}

TEST(ParserTest, InvalidClientName) {
    TempFile tf(R"(2
09:00 18:00
50
10:00 1 client@name
)");
    
    std::vector<std::unique_ptr<Event>> events;
    EXPECT_THROW({
        Parser::parse_file(tf.path(), events);
    }, std::invalid_argument);
}

TEST(ParserTest, EmptyFile) {
    TempFile tf("");
    std::vector<std::unique_ptr<Event>> events;
    EXPECT_THROW({
        Parser::parse_file(tf.path(), events);
    }, std::invalid_argument);
}

TEST(EventFactoryTest, ParseAllEventTypes) {
    EXPECT_NO_THROW(EventFactory::parse_line("10:30 1 valid_name", 1));
    EXPECT_NO_THROW(EventFactory::parse_line("10:30 2 valid_name 3", 1));
    EXPECT_NO_THROW(EventFactory::parse_line("10:30 3 valid_name", 1));
    EXPECT_NO_THROW(EventFactory::parse_line("10:30 4 valid_name", 1));
    
    EXPECT_THROW(EventFactory::parse_line("10:30 5 invalid", 1), std::invalid_argument);
    EXPECT_THROW(EventFactory::parse_line("10:30 1", 1), std::invalid_argument);
    EXPECT_THROW(EventFactory::parse_line("10:30 2 name", 1), std::invalid_argument);
}