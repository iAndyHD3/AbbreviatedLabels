#include <Geode/Geode.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <fmt/format.h>
#include <optional>
#include <Geode/utils/casts.hpp>

using namespace geode::prelude;


std::string abbreviateNumber(int num)
{
    double n = static_cast<double>(num);
    char suffix = 0;
    if (num >= 1000000)
	{
        n /= 1000000;
        suffix = 'M';
    }
	else if (num >= 1000)
	{
        n /= 1000;
        suffix = 'K';
    }
	else
	{
		return fmt::format("{}", num);
	}
	
    return fmt::format("{:.1f}{}", n, suffix);
}

std::optional<float> scaleForSuffix(char suffix)
{
	switch(suffix)
	{
		case 'K': return 1.1f;
		case 'M': return 1.5f;
		default: return {};
	}
}

struct ScaleParams : public CCObject
{
	
	float oldScale;
	float newscale;
	
	ScaleParams(float o, float n) : oldScale(o), newscale(n)
	{
		this->autorelease();
	}
};

bool isNumber(std::string_view str)
{
	if(str.empty()) return false;
	for(const auto& c : str)
	{
		if(!std::isdigit(c))
		{
			return false;
		}
	}
	return true;
}
	
struct Callbacks
{
	
	void onLabel(CCNode* sender)
	{
		auto label = (CCLabelBMFont*)sender->getChildren()->objectAtIndex(0);
		int n = sender->getTag();
		ScaleParams* scales = reinterpret_cast<ScaleParams*>(sender->getUserObject());
		
		const char* labelstr = label->getString();
		if(isNumber({labelstr, strlen(labelstr)}))
		{
			label->CCLabelBMFont::setString(abbreviateNumber(n).c_str());
			label->setScale(scales->newscale);
		}
		else
		{
			label->CCLabelBMFont::setString(std::to_string(n).c_str());
			label->setScale(scales->oldScale);
		}
	}
};

class $modify(LevelCell)
{
	void loadCustomLevelCell()
	{
		LevelCell::loadCustomLevelCell();

		CCLayer* layer = this->m_mainLayer;
		GJGameLevel* level = this->m_level;
		int likes = level->m_likes;
		int downloads = level->m_downloads;

		bool checkDownload = downloads > 1000;
		bool checkLikes = likes > 1000;

		if (!checkDownload && !checkLikes)
			return;

		int count = layer->getChildrenCount();

		for (int i = 0; i < count; i++)
		{
			//safe get children by id
			if (auto label = geode::cast::safe_cast<CCLabelBMFont*>(layer->getChildren()->objectAtIndex(i)))
			{
				//compares label string to class member for 100% safety
				if (checkDownload && atoi(label->CCLabelBMFont::getString()) == downloads)
				{
					std::string downloadstr = abbreviateNumber(downloads);
					float scaleMult = scaleForSuffix(downloadstr.back()).value_or(1.0f);

					auto newspr = CCLabelBMFont::create(downloadstr.c_str(), "bigFont.fnt");
					float oldScale = label->getScale();
					float newscale = oldScale * scaleMult;
					newspr->setScale(newscale);

					auto btn = CCMenuItemSpriteExtra::create(newspr, this, menu_selector(Callbacks::onLabel));
					btn->setTag(downloads);
					btn->setUserObject(new ScaleParams(oldScale, newscale));

					auto menu = CCMenu::create();
					menu->setPosition(label->getPosition());
					menu->setPositionX(menu->getPositionX() + 20);
					menu->addChild(btn);
					this->addChild(menu);
					label->setVisible(false);

					continue;
				}
				else if (checkLikes && atoi(label->CCLabelBMFont::getString()) == likes)
				{
					std::string likesstr = abbreviateNumber(likes);
					float scaleMult = scaleForSuffix(likesstr.back()).value_or(1.0f);
					auto newspr = CCLabelBMFont::create(likesstr.c_str(), "bigFont.fnt");
					float oldScale = label->getScale();
					float newscale = oldScale * scaleMult;
					newspr->setScale(newscale);

					auto btn = CCMenuItemSpriteExtra::create(newspr, this, menu_selector(Callbacks::onLabel));
					btn->setTag(likes);
					btn->setUserObject(new ScaleParams(oldScale, newscale));

					auto menu = CCMenu::create();
					menu->setPosition(label->getPosition());
					menu->setPositionX(menu->getPositionX() + 20);
					menu->addChild(btn);
					this->addChild(menu);
					label->setVisible(false);

				}
			}
		}
	}

};

