/*
 Copyright (C) 2010-2016 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>
#include <algorithm>

#include "Relation.h"

namespace TrenchBroom {
    TEST(RelationTest, testInsertSingle) {
        typedef relation<size_t, std::string> Relation;
        
        Relation r;
        ASSERT_TRUE(r.insert(1, "a"));
        
        ASSERT_FALSE(r.empty());
        ASSERT_TRUE(r.contains(1, "a"));
        ASSERT_EQ(1u, r.count_left("a"));
        ASSERT_EQ(1u, r.count_right(1));
        
        ASSERT_FALSE(r.insert(1, "a"));
        
        ASSERT_TRUE(r.insert(1, "b"));
        ASSERT_TRUE(r.contains(1, "b"));
        ASSERT_EQ(1u, r.count_left("a"));
        ASSERT_EQ(1u, r.count_left("b"));
        ASSERT_EQ(2u, r.count_right(1));
        
        ASSERT_TRUE(r.insert(2, "b"));
        ASSERT_EQ(1u, r.count_left("a"));
        ASSERT_EQ(2u, r.count_left("b"));
        ASSERT_EQ(2u, r.count_right(1));
        ASSERT_EQ(1u, r.count_right(2));
    }
    
    TEST(RelationTest, testInsertMultipleRight) {
        typedef relation<size_t, std::string> Relation;
        
        Relation r;
        
        const size_t left_1 = 1;
        std::vector<std::string> right_1;
        right_1.push_back("a");
        right_1.push_back("b");
        
        r.insert(left_1, right_1.begin(), right_1.end());

        ASSERT_TRUE(r.contains(left_1, right_1[0]));
        ASSERT_TRUE(r.contains(left_1, right_1[1]));
        ASSERT_EQ(1u, r.count_left(right_1[0]));
        ASSERT_EQ(1u, r.count_left(right_1[1]));
        ASSERT_EQ(2u, r.count_right(left_1));
        ASSERT_TRUE(std::equal(right_1.begin(), right_1.end(), r.right_begin(left_1)));
        
        const size_t left_2 = 2;
        std::vector<std::string> right_2;
        right_2.push_back("b");
        right_2.push_back("c");
        
        r.insert(left_2, right_2.begin(), right_2.end());
        ASSERT_TRUE(r.contains(left_2, right_2[0]));
        ASSERT_TRUE(r.contains(left_2, right_2[1]));
        ASSERT_EQ(2u, r.count_left(right_2[0]));
        ASSERT_EQ(1u, r.count_left(right_2[1]));
        ASSERT_EQ(2u, r.count_right(left_2));
        ASSERT_TRUE(std::equal(right_2.begin(), right_2.end(), r.right_begin(left_2)));
    }
    
    TEST(RelationTest, testInsertMultipleLeft) {
        typedef relation<std::string, size_t> Relation;
        
        Relation r;
        
        std::vector<std::string> left_1;
        left_1.push_back("a");
        left_1.push_back("b");
        const size_t right_1 = 1;
        
        r.insert(left_1.begin(), left_1.end(), right_1);
        
        ASSERT_TRUE(r.contains(left_1[0], right_1));
        ASSERT_TRUE(r.contains(left_1[1], right_1));
        ASSERT_EQ(1u, r.count_right(left_1[0]));
        ASSERT_EQ(1u, r.count_right(left_1[1]));
        ASSERT_EQ(2u, r.count_left(right_1));
        ASSERT_TRUE(std::equal(left_1.begin(), left_1.end(), r.left_begin(right_1)));
        
        std::vector<std::string> left_2;
        left_2.push_back("b");
        left_2.push_back("c");
        const size_t right_2 = 2;
        
        r.insert(left_2.begin(), left_2.end(), right_2);
        
        ASSERT_TRUE(r.contains(left_2[0], right_2));
        ASSERT_TRUE(r.contains(left_2[1], right_2));
        ASSERT_EQ(2u, r.count_right(left_2[0]));
        ASSERT_EQ(1u, r.count_right(left_2[1]));
        ASSERT_EQ(2u, r.count_left(right_2));
        ASSERT_TRUE(std::equal(left_2.begin(), left_2.end(), r.left_begin(right_2)));
    }
    
    TEST(RelationTest, testEraseSingle) {
        typedef relation<size_t, std::string> Relation;
        
        Relation r;
        r.insert(1, "a");
        r.insert(1, "b");
        r.insert(2, "b");
        r.insert(3, "c");
        
        // just to make sure
        ASSERT_TRUE(r.contains(1, "a"));
        ASSERT_TRUE(r.contains(1, "b"));
        ASSERT_TRUE(r.contains(2, "b"));
        ASSERT_TRUE(r.contains(3, "c"));

        ASSERT_FALSE(r.erase(3, "a"));
        ASSERT_FALSE(r.erase(4, ""));
        ASSERT_FALSE(r.erase(3, "a"));
        
        ASSERT_TRUE(r.erase(1, "a"));
        ASSERT_FALSE(r.contains(1, "a"));
        ASSERT_TRUE(r.contains(1, "b"));
        ASSERT_TRUE(r.contains(2, "b"));
        ASSERT_TRUE(r.contains(3, "c"));
        ASSERT_FALSE(r.erase(1, "a"));
        
        ASSERT_TRUE(r.erase(3, "c"));
        ASSERT_FALSE(r.contains(1, "a"));
        ASSERT_TRUE(r.contains(1, "b"));
        ASSERT_TRUE(r.contains(2, "b"));
        ASSERT_FALSE(r.contains(3, "c"));
        ASSERT_FALSE(r.erase(3, "c"));
        
        ASSERT_TRUE(r.erase(1, "b"));
        ASSERT_FALSE(r.contains(1, "a"));
        ASSERT_FALSE(r.contains(1, "b"));
        ASSERT_TRUE(r.contains(2, "b"));
        ASSERT_FALSE(r.contains(3, "c"));
        ASSERT_FALSE(r.erase(1, "b"));
        
        ASSERT_TRUE(r.erase(2, "b"));
        ASSERT_FALSE(r.contains(1, "a"));
        ASSERT_FALSE(r.contains(1, "b"));
        ASSERT_FALSE(r.contains(2, "b"));
        ASSERT_FALSE(r.contains(3, "c"));
        ASSERT_FALSE(r.erase(2, "b"));
    }
}
