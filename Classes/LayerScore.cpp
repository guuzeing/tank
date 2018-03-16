#include "LayerScore.h"


bool LayerScore::init()
{
	CCLayer::init();

	// �����������http���󣬰�TotalScore��Score���ͷ�����
	// ���������ط���ǰ10�����

	int totalScore = CCUserDefault::sharedUserDefault()->getIntegerForKey("TotalScore", 0);
	int score = CCUserDefault::sharedUserDefault()->getIntegerForKey("Score", 0);
	int userid = CCRANDOM_0_1() * 100; // user99

	// ׼��URL
	char url[2048];
	sprintf(url, "http://192.168.192.130/cgi-bin/setScore.cgi?totalscore=%d&score=%d&user=user%d", 
		totalScore, score, userid);

	// ���������������
	CCHttpClient* client = CCHttpClient::getInstance();

	CCHttpRequest* req = new CCHttpRequest;
	req->setUrl(url);
	req->setRequestType(CCHttpRequest::kHttpGet);
	req->setResponseCallback(this, httpresponse_selector(LayerScore::HttpResponse));
	
	client->send(req);
	req->release();

	// ������10����ʾ�Ŀؼ�
	for (int i = 0; i < 10; i++)
	{
		CCLabelTTF* label = CCLabelTTF::create("nouser", "Arial", 24);
		addChild(label);
		label->setTag(1000 + i);
		label->setPosition(ccp(winSize.width / 2, winSize.height));
		Common::moveNodeY(label, -i * 30 - 50);
	}

	return true;
}

void LayerScore::HttpResponse(CCHttpClient* client, CCHttpResponse* response)
{
	// user1&1000&100&user2&1000&100
	if (!response->isSucceed())
	{
		CCLog("request error:%s", response->getErrorBuffer());
		return;
	}

	std::vector<char>* data = response->getResponseData();
	
	std::string str;
	std::vector<char>::iterator it;
	for (it = data->begin(); it != data->end(); ++it)
	{
		str.push_back(*it);
	}

	char* p = new char[str.size() + 1];
	strcpy(p, str.c_str());
	p[strlen(p) - 1] = 0; // ����&ȥ��
	int index = 0;
	
	char* username = strtok(p, "&");
	char* totalscore;
	char* score;
	char buf[1024];
	while (username)
	{
		totalscore = strtok(NULL, "&");
		score = strtok(NULL, "&");
		CCLog("**********%s, %s, %s", username, totalscore, score);

		// �����ݷŵ��ؼ���
		CCLabelTTF* label = (CCLabelTTF*)getChildByTag(1000 + index++);
		sprintf(buf, "%s:%s:%s", username, totalscore, score);
		label->setString(buf);

		username = strtok(NULL, "&");
	}

	delete[]p;

	
}

