#include <stdlib.h>

#include <vector>
#include <string>

#include <gtest/gtest.h>

#include "../util.hpp"
#include "../apr_table.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;
using modruby::split;
using modruby::url_decode;
using modruby::apr::table;
using modruby::apr::pool;

const char* query_args = "city=fort+worth&fb_sig_in_profile_tab=1&fb_sig_profile_user=1111111111&fb_sig_profile_id=1111111111&fb_sig_in_canvas=1&fb_sig_request_method=GET&fb_sig_friends=7906051%2C23902556%2C25300027%2C157001315%2C509681381%2C511243068%2C525218275%2C529587851%2C555197650%2C570236526%2C577508938%2C594418116%2C617837346%2C622793547%2C630330994%2C646559812%2C655300229%2C655307063%2C688522939%2C700565120%2C710627207%2C717699232%2C728659031%2C758508199%2C780431406%2C804338451%2C1007566198%2C1101671709%2C1130252654%2C1156366161%2C1196854701%2C1254541942%2C1258311941%2C1260065200%2C1276523432%2C1309421167%2C1313718233%2C1314025723%2C1319414931%2C1331634051%2C1355588872%2C1376053520%2C1406043530%2C1418756297%2C1420174777%2C1450206572%2C1454780440%2C1488776843%2C1497760051%2C1503671235%2C1576571763%2C1603415483%2C1631640440%2C1753684402%2C100000094167049%2C100000110630901%2C100000199959170%2C100000232283536&fb_sig_locale=en_US&fb_sig_in_new_facebook=1&fb_sig_time=1280863740.6628&fb_sig_added=1&fb_sig_profile_update_time=1268068036&fb_sig_expires=1280863800&fb_sig_profile_session_key=11111111111111111111111111111111111111111111111111111111111111111111111111111&fb_sig_ext_perms=user_birthday%2Cuser_religion_politics%2Cuser_relationships%2Cuser_relationship_details%2Cuser_hometown%2Cuser_location%2Cuser_likes%2Cuser_activities%2Cuser_interests%2Cuser_education_history%2Cuser_work_history%2Cuser_online_presence%2Cuser_website%2Cuser_groups%2Cuser_events%2Cuser_photos%2Cuser_videos%2Cuser_photo_video_tags%2Cuser_notes%2Cuser_about_me%2Cuser_status%2Cfriends_birthday%2Cfriends_religion_politics%2Cfriends_relationships%2Cfriends_relationship_details%2Cfriends_hometown%2Cfriends_location%2Cfriends_likes%2Cfriends_activities%2Cfriends_interests%2Cfriends_education_history%2Cfriends_work_history%2Cfriends_online_presence%2Cfriends_website%2Cfriends_groups%2Cfriends_events%2Cfriends_photos%2Cfriends_videos%2Cfriends_photo_video_tags%2Cfriends_notes%2Cfriends_about_me%2Cfriends_status&fb_sig_country=us&fb_sig_api_key=eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee&fb_sig_app_id=111111111111&fb_sig=eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";

class UnitTest : public testing::Test
{
  public:

    UnitTest() : Test()
    {
        my_data = 101;
    }

    virtual ~UnitTest()
    {

    }

  protected:  

    int my_data;

    void testMethod()
    {
        ASSERT_EQ(my_data, 101);
    }
};

// Tests request::parse_queries
TEST_F(UnitTest, 1)
{
    vector<string> pairs;
    split(query_args, '&', pairs);

    table t(pool());

    std::vector<std::string>::iterator i;
    for(i = pairs.begin(); i != pairs.end(); i++)
    {
        unsigned found = i->find("=");

        if(found != string::npos)
        {
            string key   = url_decode(i->substr(0, found).c_str());
            string value = url_decode(i->substr(found+1, i->size() - 1).c_str());

            //cout << key << ": " << value << endl;

            t.merge(key.c_str(), value.c_str());
        }
    }

    modruby::apr::table::iterator pair(t);

    while(pair.next())
    {
        string key = pair.key();
        string val = pair.data();

        printf("%s: %s\n", key.c_str(), val.c_str());
    }

}

int main(int argc, char **argv)
{
    modruby::apr::init();

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
