#pragma once
#include "strategy.h"
#include <vector>
#include "parameter.h"
//#include "hwcout.h"

class strategy_baiyu_yng4 :public Strategy {
	//	基类拥有这些属性, 不必重复定义
	// int wbNum = 0;			工作台数量, 在readMap时赋值, 不会更新
	// hwRobot botArr[4];		bot数组, 包含4个bot实例, bot成员函数参考hwRobot.h, 在readMap时赋值
	// hwWorkbench wbArr[50];	工作台实例, 有wbNum个, 在readMap时赋值, 工作台顺序不会改变
	// int frameId = 0;			当前帧ID, 0表示正在读取地图, [1,9000]. 每帧更新一次;
	// int money = 200000;		拥有的资金, 每帧更新一次

	vector<vector<double>> wbDis;
	vector<vector<double>> wbDisSq;
	vector<vector<int>> wbTypeArr = vector<vector<int>>(10, vector<int>());

	// 第0项表示输入端是否被作为卖出的目标, 标识工作台的输入端被承载哪种类型货物的机器人谁选中，二进制位表示，例如，0b110，意味着它的输入端被持有1号货物和2号货物的机器人锁定。
	// 第1项表示是否被作为买入的目标
	vector<vector<int>> wbIfTar{ 50,vector<int>(2, 0) };

	// 第0项表示目前移动到第几个目标, 值为0表示无目标, 值为1表示第一个目标(买入, 值为2表示第二个目标(卖出
	// 第1项, 第2项为 两个目标的wbid
	vector<vector<int>> botTarId{ 4, vector<int>(3, 0) };
	// 当前某种种类的产品被需要的程度，关系到购入的选择
	char productNeed[8];
	// 各类型工作台的个数
	int n1, n2, n3, n4, n5, n6, n7, n8, n9;
	// 标识上一次状态是否成功更新
	bool successUpdate[4] = { false };

	// 特殊余帧flag, 确保只执行一次
	bool specialResFrameFlag = false;

	//const int valueArr[8] = { 0,3000,3200,3400,7100,7800,8300,29000 };
	const int valueArr[8] = { 0,4000,4000,4000,7000,7000,7000,29000 };

	const int weightOffset[8] = { 0,3000,3000,3000,168000,22000,22000,29000 };

	//const int valueArr[8] = { 0,4000,4000,4000,7000,7000,7000,29000 };

	void initProductNeed(char* productNeed) {

		for (int i = 0; i < 8; ++i)
			productNeed[i] = 0;
		if (n9 >= 1)     // 这个if-else语句，影响的是机器人的购入操作，看它购入的商品是否被需要，如不不需要，则不购入
			for (int i = 1; i < 8; ++i)
				productNeed[i] = 10;
		else {
			if (n8 >= 1)
				productNeed[7] = 10;
			for (int i = 0; i < n7; ++i)
				for (int j = 4; j <= 6; ++j)
					if (wbArr[wbTypeArr[7][i]].ifNeed(j))
						++productNeed[j];
			for (int i = 0; i < n6; ++i) {
				if (wbArr[wbTypeArr[6][i]].ifNeed(3))
					++productNeed[3];
				if (wbArr[wbTypeArr[6][i]].ifNeed(2))
					++productNeed[2];
			}
			for (int i = 0; i < n5; ++i) {
				if (wbArr[wbTypeArr[5][i]].ifNeed(3))
					++productNeed[3];
				if (wbArr[wbTypeArr[5][i]].ifNeed(1))
					++productNeed[1];
			}
			for (int i = 0; i < n4; ++i) {
				if (wbArr[wbTypeArr[4][i]].ifNeed(2))
					++productNeed[2];
				if (wbArr[wbTypeArr[4][i]].ifNeed(1))
					++productNeed[1];
			}
		}

	}

	const double calDist(const double x1, const double y1, const double x2, const double y2)const {
		return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	}
	const double calDistSq(const double x1, const double y1, const double x2, const double y2)const {
		return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
	}

	void initDisV2AndWbTypeArr() {
		wbDis.resize(wbNum);
		wbDisSq.resize(wbNum);
		for (auto& one : wbDis) {
			one.resize(wbNum, -1);
		}
		for (auto& one : wbDisSq) {
			one.resize(wbNum, -1);
		}
		for (int i = 0; i < wbNum; ++i) {
			wbTypeArr[wbArr[i].wbType].emplace_back(i);

			for (int j = 0; j < i; ++j) {
				int jType = wbArr[j].wbType;
				bool dis_flag = false;
				switch (wbArr[i].wbType)
				{
				case 1:
					if (jType == 4 || jType == 5 || jType == 9) dis_flag = true;
					break;
				case 2:
					if (jType == 4 || jType == 6 || jType == 9) dis_flag = true;
					break;
				case 3:
					if (jType == 5 || jType == 6 || jType == 9) dis_flag = true;
					break;
				case 4:
					if (jType == 1 || jType == 2 || jType == 7 || jType == 9) dis_flag = true;
					break;
				case 5:
					if (jType == 1 || jType == 3 || jType == 7 || jType == 9) dis_flag = true;
					break;
				case 6:
					if (jType == 2 || jType == 3 || jType == 7 || jType == 9) dis_flag = true;
					break;
				case 7:
					if (jType == 4 || jType == 5 || jType == 6 || jType == 8 || jType == 9) dis_flag = true;
					break;
				case 8:
					if (jType == 7) dis_flag = true;
					break;
				case 9:
					if (jType != 8 && jType != 9) dis_flag = true;
					break;
				default:
					break;
				}
				if (dis_flag) {
					double _disSq = calDistSq(wbArr[i].x, wbArr[i].y, wbArr[j].x, wbArr[j].y);
					wbDisSq[i][j] = _disSq;
					wbDisSq[j][i] = _disSq;

					double _dis = sqrt(_disSq);
					wbDis[i][j] = _dis;
					wbDis[j][i] = _dis;
				}
			}
		}
	}

	// botId 去wbid1 取产品goodid 送到wbId2 的 权重值
	double getWeight(int botId, int wbId1, int wbId2, int goodId) {
		if (wbDis[wbId1][wbId2] == -1) {
			return 0;
		}
		double value = valueArr[goodId];
		int mstate = wbArr[wbId2].materialsState;
		for (int i = 0; i < 8; ++i) {
			if (mstate & 1) {		// 判断wbArr[wbid2]需不需要这个材料
				value += valueArr[i];	// 为了补全另外加的权重，如果wbArr[wbid2]当前拥有的材料越多，它的权重就越大，如果它不缺材料，会有相应的机制处理
			}
			mstate = mstate >> 1;
		}

		double _disBot2Wb1 = calDist(botArr[botId].x, botArr[botId].y, wbArr[wbId1].x, wbArr[wbId1].y);
		return pow(value, 1.0) / (pow(_disBot2Wb1 + wbDis[wbId1][wbId2] / 2, 1.0));
	}

	// botId将产品goodid送到wbId2的权重值
	double getWeight2(int botId, int id2, int goodId) {

		double value = valueArr[goodId];
		int mstate = wbArr[id2].materialsState;	// 对于工作台8和9，它拥有的原材料可以说一直是0，让它吃亏点
		for (int i = 0; i < 8; ++i) {
			if (mstate & 1) {		// 判断wbArr[id2]需不需要这个材料
				value += valueArr[i] / 2;	// 为了补全另外加的权重，如果wbArr[id2]当前拥有的材料越多，它的权重就越大，如果它不缺材料，会有相应的机制处理
			}
			mstate = mstate >> 1;
		}

		double _disBot2Wb2 = calDist(botArr[botId].x, botArr[botId].y, wbArr[id2].x, wbArr[id2].y);
		return value / _disBot2Wb2;
	}
	// 根据 所运送货品的 需求程度的价格补正
	double getWeightOffset(int botId, int wbId1, int wbId2, int goodId) {
		int typeId2 = wbArr[wbId2].wbType;

		int pn = productNeed[typeId2];

		double n = 0;
		switch (typeId2)
		{
		case 1:
			n = n4 + n5;
			break;
		case 2:
			n = n4 + n6;
			break;
		case 3:
			n = n5 + n6;
			break;
		case 4:
			n = n7 + n9;
			break;
		case 5:
			n = n7 + n9;
			break;
		case 6:
			n = n7 + n9;
			break;
		case 7:
			n = n8 + n9;
			break;
		default:
			break;
		}
		double offset = 0;
		if (goodId == 7) {
			offset = weightOffset[goodId];
		}
		else {
			offset = pn / n * weightOffset[typeId2];
		}
		return offset;

	}

	// 根据货物需求程度的价格补正
	double getWeightOffset2(int botId, int wbId1, int wbId2, int goodId) {
		int typeId2 = wbArr[wbId2].wbType;

		int pn = productNeed[typeId2];

		double n = 0;
		switch (typeId2)
		{
		case 1:
			n = n4 + n5;
			break;
		case 2:
			n = n4 + n6;
			break;
		case 3:
			n = n5 + n6;
			break;
		case 4:
			n = n7 + n9;
			break;
		case 5:
			n = n7 + n9;
			break;
		case 6:
			n = n7 + n9;
			break;
		case 7:
			n = n8 + n9;
			break;
		default:
			break;
		}
		double offset = 0;
		if (goodId == 7) {
			offset = weightOffset[goodId];
		}
		else {
			offset = pn / n * weightOffset[typeId2];
		}


		return offset;

	}


	// 在当前地图中，根据getWeight()计算的权重，查找botID最适合拿来进货的工作台wbArr[id1]和最适合拿来出货的工作台wbArr[id2]
	int loopId1ToId2(int tp1, int tp2, int botId, int goodId, int& wbId1, int& wbId2, double& maxWeight) {

		if (productNeed[tp1] <= 0)		// 查看当前tp1类型的货物是否需要，涉及购入操作
			return 0;

		// 遍历8号工作台是否有需要？
		for (int id2 : wbTypeArr[tp2]) {
			// wbArr[id2]是否需要该tp1商品, 是否被作为卖出的目标（作为被卖出的目标的话，就不选它）
			if (wbArr[id2].ifNeed(tp1) && !(wbIfTar[id2][0] & (1 << tp1))) {
				for (int id1 : wbTypeArr[tp1]) {

					double x = botArr[botId].x, y = botArr[botId].y;

					// 如果给去7拿货, 另外判定自身是否在7附近, 采用粗略判定
					if (tp1 == 7)
						if (fabs(x - wbArr[id1].x) > 0.4001 || fabs(y - wbArr[id1].y) > 0.4001)
							continue;

					if (tp1 == 4)
						if (fabs(x - wbArr[id1].x) > 0.4001 || fabs(y - wbArr[id1].y) > 0.4001)
							continue;

					//switch (tp1)
					//{
					//case 7:
					//	if (fabs(x - wbArr[id1].x) > 0.4001 || fabs(y - wbArr[id1].y) > 0.4001)
					//		continue;
					//	break;
					////case 6:
					////case 5:
					//case 4:
					//	if (fabs(x - wbArr[id1].x) > 0.4001 || fabs(y - wbArr[id1].y) > 0.4001)
					//		continue;
					//	break;

					//default:
					//	break;
					//}

					double dist = calDist(x, y, wbArr[id1].x, wbArr[id1].y);
					bool checkProduct = wbArr[id1].ifHaveProduct;
					if (frameId < 50 && wbArr[id1].rpt > 0 && dist > (wbArr[id1].rpt * 0.02 * 6))
						checkProduct = true;

					//double x = botArr[botId].x, y = botArr[botId].y;
					//double dist = calDist(x, y, wbArr[id1].x, wbArr[id1].y);
					//bool checkProduct = wbArr[id1].ifHaveProduct;
					//if (wbArr[id1].rpt > 0 && dist > ((wbArr[id1].rpt) * 0.02 * 6) + 0.4) {
					//	checkProduct = true;
					//}

					//if (wbArr[id1].ifHaveProduct && !wbIfTar[id1][1]) {		// 是否即将拥有tp2商品, 是否被作为买入的目标
					if (checkProduct && !wbIfTar[id1][1]) {		// 是否即将拥有tp2商品, 是否被作为买入的目标
						double nowWeight = getWeight(botId, id1, id2, goodId) + getWeightOffset2(botId, id1, id2, goodId);

						if (nowWeight > maxWeight) {
							maxWeight = nowWeight;
							wbId1 = id1;
							wbId2 = id2;
						}
					}
				}
			}
		}
		return 0;
	};

	// 在当前地图中，根据getWeight()计算的权重，查找botID最适合拿来sell的工作台wbArr[wb2Id]
	void loopWbId2(int botId, int goodId, int wb2Type, int& wb2Id, double& maxWeight) {

		// 无需检查productNeeed，因为保证goodId被需要
		for (int id2 : wbTypeArr[wb2Type]) {
			if (wbArr[id2].ifNeed(goodId) && !(wbIfTar[id2][0] & (1 << goodId))) {
				double nowWeight = getWeight2(botId, id2, goodId);
				if (nowWeight > maxWeight) {
					maxWeight = nowWeight;
					wb2Id = id2;
				}
			}
		}
	}

	// 试图找到最佳的buy工作台 ，且在这里不考虑工作台有无被其它机器人锁定的事情
	int findBestWbBuy(int botId, int goodId, double& maxWeight) {

		int tarId = -1;
		int id2 = botTarId[botId][2];
		for (int id1 : wbTypeArr[goodId]) {
			if (wbArr[id1].ifHaveProduct) {		// 是否拥有goodId商品
				double nowWeight = getWeight(botId, id1, id2, goodId);
				if (nowWeight > maxWeight) {
					maxWeight = nowWeight;
					tarId = id1;
				}
			}
		}
		return tarId;

	};

	int findNext(int botId) {
		int wbId1 = -1;
		int wbId2 = -1;

		double maxWeight = 0;

		loopId1ToId2(7, 8, botId, 7, wbId1, wbId2, maxWeight);

		loopId1ToId2(6, 7, botId, 6, wbId1, wbId2, maxWeight);
		loopId1ToId2(5, 7, botId, 5, wbId1, wbId2, maxWeight);
		loopId1ToId2(4, 7, botId, 4, wbId1, wbId2, maxWeight);

		loopId1ToId2(3, 6, botId, 3, wbId1, wbId2, maxWeight);
		loopId1ToId2(2, 6, botId, 2, wbId1, wbId2, maxWeight);

		loopId1ToId2(3, 5, botId, 3, wbId1, wbId2, maxWeight);
		loopId1ToId2(1, 5, botId, 1, wbId1, wbId2, maxWeight);

		loopId1ToId2(2, 4, botId, 2, wbId1, wbId2, maxWeight);
		loopId1ToId2(1, 4, botId, 1, wbId1, wbId2, maxWeight);


		loopId1ToId2(7, 9, botId, 7, wbId1, wbId2, maxWeight);
		loopId1ToId2(6, 9, botId, 6, wbId1, wbId2, maxWeight);
		loopId1ToId2(5, 9, botId, 5, wbId1, wbId2, maxWeight);
		loopId1ToId2(4, 9, botId, 4, wbId1, wbId2, maxWeight);
		loopId1ToId2(3, 9, botId, 3, wbId1, wbId2, maxWeight);
		loopId1ToId2(2, 9, botId, 2, wbId1, wbId2, maxWeight);
		loopId1ToId2(1, 9, botId, 1, wbId1, wbId2, maxWeight);

		if (wbId1 != -1 && wbId2 != -1) {
			botTarId[botId] = { 1,wbId1,wbId2 };
			wbIfTar[wbId1][1] = 1;

			if (wbArr[wbId2].wbType != 8 && wbArr[wbId2].wbType != 9)
				wbIfTar[wbId2][0] = (wbIfTar[wbId2][0] | (1 << wbArr[wbId1].wbType));
			//hwcout << "botid:" << botId << "wbId1:" << wbId1 << " wbId2:" << wbId2 << " maxWeight:" << maxWeight << hwendl;
			return 0;
		}
		else {
			botTarId[botId] = { 0, 0, 0 };
			return 1;
		}

	}

	void findNext2(int botId, int& wbId1, int& wbId2, double& maxWeight) {

		if (botArr[botId].gootsId)		// 如果机器人目前携带着货物，就不更新
			return;

		loopId1ToId2(7, 8, botId, 7, wbId1, wbId2, maxWeight);

		loopId1ToId2(6, 7, botId, 6, wbId1, wbId2, maxWeight);
		loopId1ToId2(5, 7, botId, 5, wbId1, wbId2, maxWeight);
		loopId1ToId2(4, 7, botId, 4, wbId1, wbId2, maxWeight);

		loopId1ToId2(3, 6, botId, 3, wbId1, wbId2, maxWeight);
		loopId1ToId2(2, 6, botId, 2, wbId1, wbId2, maxWeight);

		loopId1ToId2(3, 5, botId, 3, wbId1, wbId2, maxWeight);
		loopId1ToId2(1, 5, botId, 1, wbId1, wbId2, maxWeight);

		loopId1ToId2(2, 4, botId, 2, wbId1, wbId2, maxWeight);
		loopId1ToId2(1, 4, botId, 1, wbId1, wbId2, maxWeight);


		loopId1ToId2(7, 9, botId, 7, wbId1, wbId2, maxWeight);
		loopId1ToId2(6, 9, botId, 6, wbId1, wbId2, maxWeight);
		loopId1ToId2(5, 9, botId, 5, wbId1, wbId2, maxWeight);
		loopId1ToId2(4, 9, botId, 4, wbId1, wbId2, maxWeight);
		loopId1ToId2(3, 9, botId, 3, wbId1, wbId2, maxWeight);
		loopId1ToId2(2, 9, botId, 2, wbId1, wbId2, maxWeight);
		loopId1ToId2(1, 9, botId, 1, wbId1, wbId2, maxWeight);
	}

	void findNext3(int botId, int goodId, int& wb2Id, double& maxWeight) {

		if (goodId == 1) {
			loopWbId2(botId, 1, 4, wb2Id, maxWeight);
			loopWbId2(botId, 1, 5, wb2Id, maxWeight);
			loopWbId2(botId, 1, 9, wb2Id, maxWeight);
		}
		else if (goodId == 2) {
			loopWbId2(botId, 2, 4, wb2Id, maxWeight);
			loopWbId2(botId, 2, 6, wb2Id, maxWeight);
			loopWbId2(botId, 2, 9, wb2Id, maxWeight);
		}
		else if (goodId == 3) {
			loopWbId2(botId, 3, 5, wb2Id, maxWeight);
			loopWbId2(botId, 3, 6, wb2Id, maxWeight);
			loopWbId2(botId, 3, 9, wb2Id, maxWeight);
		}
		else if (goodId == 4) {
			loopWbId2(botId, 4, 7, wb2Id, maxWeight);
			loopWbId2(botId, 4, 9, wb2Id, maxWeight);
		}
		else if (goodId == 5) {
			loopWbId2(botId, 5, 7, wb2Id, maxWeight);
			loopWbId2(botId, 5, 9, wb2Id, maxWeight);
		}
		else if (goodId == 6) {
			loopWbId2(botId, 6, 7, wb2Id, maxWeight);
			loopWbId2(botId, 6, 9, wb2Id, maxWeight);
		}
		else if (goodId == 7) {
			loopWbId2(botId, 7, 8, wb2Id, maxWeight);
			loopWbId2(botId, 7, 9, wb2Id, maxWeight);
		}
	}

	void avoidCrash() {

		int crashNum[4] = { 0 };
		int crashId[4][4] = { {-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1},{-1,-1,-1,-1} };
		double vi_par, vi_ver, vj_par, vj_ver, xi, yi, xj, yj, boti_vx, boti_vy, botj_vx, botj_vy, angleTowPoint;
		double vij_par, vij_ver, botk_vx, botk_vy, botm_vx, botm_vy, cosAngle, sinAngle, dist, t_catch;
		double ri, rj, face_to_i, face_to_j;
		bool rotate_dir[4][4];		// 值为true，表示两者都应该逆时针旋转，值为false，表示两者都应该顺时针旋转
		bool rotate_flag[4];		// 值为true，表示应该旋转，值为false，表示不应该旋转，防止后退的时候旋转

		// 碰撞检测模块（目前只检测钝角相撞）
		for (int i = 0; i < 4; ++i) {
			boti_vx = botArr[i].lineSpeed[0];
			boti_vy = botArr[i].lineSpeed[1];
			face_to_i = botArr[i].faceTo;

			double deltaAngle_i = fabs(atan2(boti_vy, boti_vx) - face_to_i);
			if (deltaAngle_i > M_PI) {
				deltaAngle_i = 2 * M_PI - deltaAngle_i;
			}

			if (deltaAngle_i > M_PI_2) {	// 说明这时候是在做后退运动
				rotate_flag[i] = false;
			}
			else {
				rotate_flag[i] = true;
			}
		}

		for (int i = 0; i < 4; ++i) {

			xi = botArr[i].x;
			yi = botArr[i].y;
			boti_vx = botArr[i].lineSpeed[0];
			boti_vy = botArr[i].lineSpeed[1];
			face_to_i = botArr[i].faceTo;
			if (botArr[i].gootsId)
				ri = 0.53;
			else
				ri = 0.45;

			for (int j = i + 1; j < 4; ++j) {

				xj = botArr[j].x;
				yj = botArr[j].y;
				dist = calDist(xi, yi, xj, yj);
				botj_vx = botArr[j].lineSpeed[0];
				botj_vy = botArr[j].lineSpeed[1];
				face_to_j = botArr[j].faceTo;
				if (botArr[j].gootsId)
					rj = 0.53;
				else
					rj = 0.45;

				if (botj_vx * boti_vx + botj_vy * boti_vy > 0)		// 向量的点积，同向运动的先不管
					continue;

				double delta_vx = botj_vx - boti_vx, delta_vy = botj_vy - boti_vy;
				double deltax = xj - xi, deltay = yj - yi;

				if (delta_vx * deltax + delta_vy * deltay > 0)	// 向量的点积，检查相对位置向量与相对速度向量是否反向（成钝角），反向则说明两个机器人正在靠近
					continue;

				angleTowPoint = atan2(yj - yi, xj - xi);
				cosAngle = cos(angleTowPoint);
				sinAngle = sin(angleTowPoint);

				vi_par = boti_vx * cosAngle + boti_vy * sinAngle;	// i在两点连线的水平速度
				vi_ver = boti_vx * sinAngle - boti_vy * cosAngle;	// i在两点连线的垂直速度
				vj_par = botj_vx * cosAngle + botj_vy * sinAngle;	// j在两点连线的水平速度
				vj_ver = botj_vx * sinAngle - botj_vy * cosAngle;	// j在两点连线的垂直速度

				vij_par = vj_par - vi_par;	// 从i看j，j向i直接靠近的速度
				vij_ver = vj_ver - vi_ver;	// 从i看j，j向i垂直远离的速度

				t_catch = dist / vij_par;

				if (rotate_flag[i] && rotate_flag[j]) {	// 两者都没有做后退运动
					if (fabs(t_catch) > f4_crash_t)	// 碰撞时间还剩比较多的话，不作处理
						continue;
				}
				else {	// 其中有一方在做后退运动
					if (fabs(t_catch) > 1 * f4_crash_t)	// 碰撞时间还剩比较多的话，不作处理
						continue;
				}

				if (fabs(vij_ver * t_catch) < (ri + rj)) {	// 说明这时候它们的速度保持不变的话，过了t_catch的时间，它们就会相撞

					++crashNum[i];
					++crashNum[j];
					crashId[i][j] = 1;
					crashId[j][i] = 1;

					if (deltax * delta_vy - deltay * delta_vx < 0) {	// 向量的叉积，值为负，大家以pi的角速度旋转
						rotate_dir[i][j] = true;
						rotate_dir[j][i] = true;
					}
					else {		// 值为正，大家以-pi的角速度旋转
						rotate_dir[i][j] = false;
						rotate_dir[j][i] = false;
					}
				}
			}
		}

		// 碰撞避免模块
		for (int k = 0; k < 4; ++k) {

			if (crashNum[k] == 0)	// 没有即将要碰撞的目标，不作处理
				continue;

			botk_vx = botArr[k].lineSpeed[0];
			botk_vy = botArr[k].lineSpeed[1];

			if (crashNum[k] >= 2) {	// 如果检测得到的碰撞目标数大于等于2，则只作减速处理，不作拐弯操作
				botArr[k].forward(0.5 * sqrt(botk_vx * botk_vx + botk_vy * botk_vy));

				// 同时指示其它的机器人，不必将k视为碰撞目标，避免死锁
				for (int m = 0; m < 4; ++m) {
					if (crashId[k][m] != -1) {
						--crashNum[m];
						--crashNum[k];
						crashId[k][m] = -1;
						crashId[m][k] = -1;
					}
				}
				continue;
			}

			// 开始碰撞避免
			for (int m = 0; m < 4; ++m) {
				if (crashId[k][m] != -1) {
					botm_vx = botArr[m].lineSpeed[0];
					botm_vy = botArr[m].lineSpeed[1];
					double vk = sqrt(botk_vx * botk_vx + botk_vy * botk_vy);
					double vm = sqrt(botm_vx * botm_vx + botm_vy * botm_vy);
					if (vk > 6)
						vk = 6;
					if (vm > 6)
						vm = 6;

					if (rotate_dir[k][m]) {
						if (rotate_flag[m]) {
							botArr[m].rotate(M_PI);
							botArr[m].forward(vm);
						}
						else {	// 机器人正在后退，该机器人不进行避障
							botArr[m].rotate(botArr[m].angleSpeed);
							botArr[m].forward(-vm);
						}
						if (rotate_flag[k]) {
							botArr[k].rotate(M_PI);
							botArr[m].forward(vk);
						}
						else {	// 机器人正在后退，该机器人不进行避障
							botArr[k].rotate(botArr[k].angleSpeed);
							botArr[m].forward(-vk);
						}
					}
					else {
						if (rotate_flag[m]) {
							botArr[m].rotate(-M_PI);
							botArr[m].forward(vm);
						}
						else {	// 机器人正在后退，该机器人不进行避障
							botArr[m].rotate(botArr[m].angleSpeed);
							botArr[m].forward(-vm);
						}
						if (rotate_flag[k]) {
							botArr[k].rotate(-M_PI);
							botArr[m].forward(vk);
						}
						else {	// 机器人正在后退，该机器人不进行避障
							botArr[k].rotate(botArr[k].angleSpeed);
							botArr[m].forward(-vk);
						}

					}
					crashId[k][m] = -1;
					crashId[m][k] = -1;
					--crashNum[m];
					--crashNum[k];

					//hwcout << "frame " << frameId << hwendl;
					//hwcout << "botidk " << k << hwendl;
					//hwcout << "botidm " << m << hwendl;
					//if (rotate_flag[k])
					//	hwcout << "vk " << vk << hwendl;
					//else
					//	hwcout << "vk " << -vk << hwendl;
					//if (rotate_flag[m])
					//	hwcout << "vm " << vm << hwendl;
					//else
					//	hwcout << "vm " << -vm << hwendl;
					//if (rotate_dir[k][m])
					//	hwcout << "omega: M_PI " << hwendl;
					//else
					//	hwcout << "omega: -M_PI " << hwendl;
				}
			}

		}
	}

	void competeTar(int botId);

	/// <summary>
	/// 定义在下方
	/// </summary>
	virtual void afterReadMap();

	/// <summary>
	/// 定义在下方
	/// </summary>
	virtual void afterReadFrame(int frameId);
};

void strategy_baiyu_yng4::afterReadMap() {
	initDisV2AndWbTypeArr();
	n1 = wbTypeArr[1].size(), n2 = wbTypeArr[2].size(), n3 = wbTypeArr[3].size();
	n4 = wbTypeArr[4].size(), n5 = wbTypeArr[5].size(), n6 = wbTypeArr[6].size();
	n7 = wbTypeArr[7].size(), n8 = wbTypeArr[8].size(), n9 = wbTypeArr[9].size();
}

void strategy_baiyu_yng4::afterReadFrame(int frameId) {

	initProductNeed(productNeed);

	for (int i = 0; i < 4; ++i) {
		// 如果持有产品，那它肯定是以或者将以某个工作台为目标，这样就消耗掉1个产品需求
		if (botArr[i].gootsId > 0 && botArr[i].gootsId < 7) // 不管上一次有没有update成功，只要持有了产品，就会消耗掉1个产品需求
			--productNeed[botArr[i].gootsId];
		// 如果不持有产品，且之前能成功更新状态，且目前意图去拿某个产品(购入)，这样也消耗掉1个产品需求
		if (botArr[i].gootsId == 0 && successUpdate[i]) // 如果之前不能成功更新状态，说明它在卖出产品后，找不到下一个目标，而botTarget[i]仍然等于它卖出产品的工作台id
			--productNeed[wbArr[botTarId[i][1]].wbType];
		// 动态更新目标
		//competeTar(i);
	}

	for (int i = 0; i < 4; ++i) {

		if (i == 0 && frameId > 8602 && !specialResFrameFlag) {
			specialResFrameFlag = true;
			botTarId[i] = { 1,0,16 };
			botArr[i].setMoveTarget(wbArr[botTarId[i][1]]);

		}

		hwRobot& bot = botArr[i];
		int state = bot.moveUntillArriveTarget_YNG4();
		if (botTarId[i][0] != 0) {		// 如果此时机器人i有目标
			if (state == hwRobot::MOVE_STATE::HW_ARRIVE_TARGET) {	// 如果到达目标
				if (botTarId[i][0] == 1) {		// 如果目前到达的是要买入的目标
					if (frameId > 9000 - f4_lastN[i])
						continue;
					bot.buy();

					// 此时若生产因输出格堵塞而停止生产，那取出产品后应该进行生产。手动更改生产台的状态（rpt，materialsState，ifHaveProduct）
					if (wbArr[botTarId[i][1]].getNeed() == 0 && wbArr[botTarId[i][1]].rpt == 0) {
						wbArr[botTarId[i][1]].materialsState = 0;
						if (wbArr[botTarId[i][1]].wbType == 6) {
							++productNeed[3];
							++productNeed[2];
							wbArr[botTarId[i][1]].rpt = 500;
						}
						else if (wbArr[botTarId[i][1]].wbType == 5) {
							++productNeed[3];
							++productNeed[1];
							wbArr[botTarId[i][1]].rpt = 500;
						}
						else if (wbArr[botTarId[i][1]].wbType == 4) {
							++productNeed[2];
							++productNeed[1];
							wbArr[botTarId[i][1]].rpt = 500;
						}
						else if (wbArr[botTarId[i][1]].wbType == 7) {
							++productNeed[6];
							++productNeed[5];
							++productNeed[4];
							wbArr[botTarId[i][1]].rpt = 1000;
						}
					}

					wbArr[botTarId[i][1]].ifHaveProduct = 0;
					wbIfTar[botTarId[i][1]][1] = 0;
					botTarId[i][0] = 2;
					bot.setMoveTarget(wbArr[botTarId[i][2]]);
					bot.moveUntillArriveTarget_YNG4();

					//// 如果持有产品，那它肯定是以某个工作台为目标，这样就消耗掉1个产品需求
					//if (bot.gootsId > 0 && bot.gootsId < 7) // 不管上一次有没有update成功，只要持有了产品，就会消耗掉1个产品需求
					//	--productNeed[botArr[i].gootsId];
				}
				else if (botTarId[i][0] == 2) {	// 如果目前到达的是要售出的目标
					bot.sell();
					wbArr[bot.workbenchId].materialsState ^= (1 << bot.gootsId);

					if (wbArr[botTarId[i][2]].getNeed() == 0 && wbArr[botTarId[i][2]].rpt == -1) {  // 如果此时工作台不需要任何原料，且可以生产
						wbArr[botTarId[i][2]].materialsState = 0;     // 让它进货生产
						if (wbArr[botTarId[i][2]].wbType == 6) {
							++productNeed[3];
							++productNeed[2];
							wbArr[botTarId[i][2]].rpt = 500;
						}
						else if (wbArr[botTarId[i][2]].wbType == 5) {
							++productNeed[3];
							++productNeed[1];
							wbArr[botTarId[i][2]].rpt = 500;
						}
						else if (wbArr[botTarId[i][2]].wbType == 4) {
							++productNeed[2];
							++productNeed[1];
							wbArr[botTarId[i][2]].rpt = 500;
						}
						else if (wbArr[botTarId[i][2]].wbType == 7) {
							++productNeed[6];
							++productNeed[5];
							++productNeed[4];
							wbArr[botTarId[i][2]].rpt = 1000;
						}
					}

					// 释放工作台的goodID输入端
					if (wbArr[bot.workbenchId].wbType != 8 && wbArr[bot.workbenchId].wbType != 9)
						wbIfTar[botTarId[i][2]][0] ^= (1 << bot.gootsId);
					botTarId[i][0] = 0;
					// 手动更改机器人拥有货物的状态
					botArr[i].gootsId = 0;

					if (findNext(i) != 1) {		// 进行下一次查找，成功则出发
						successUpdate[i] = true;
						--productNeed[wbArr[botTarId[i][1]].wbType];
						bot.setMoveTarget(wbArr[botTarId[i][1]]);
						bot.moveUntillArriveTarget_YNG4();
					}
					else {					// 如果还是找不到目标，则暂时停下来
						successUpdate[i] = false;
						bot.forward(0);
					}
				}
			}
		}
		else {							// 如果此时机器人i没有目标
			if (findNext(i) != 1) {		// 进行下一次查找，成功则出发
				successUpdate[i] = true;
				--productNeed[wbArr[botTarId[i][1]].wbType];
				bot.setMoveTarget(wbArr[botTarId[i][1]]);
				bot.moveUntillArriveTarget_YNG4();
			}
			else {					// 如果还是找不到目标，则暂时停下来
				successUpdate[i] = false;
				bot.forward(0);
			}
		}
	}

	avoidCrash();
}

void strategy_baiyu_yng4::competeTar(int botId) {

	hwRobot& bot = botArr[botId];
	double penalty1 = 1.0;		// 抢别人的目标的惩罚因子
	double penalty2 = f4_penalty2;		// 改变自己的进货目标的惩罚因子
	double penalty3 = f4_penalty3;		// 改变自己的出货目标的惩罚因子
	int botIdid1 = botTarId[botId][1];	// botIdbuy目标id
	int botIdid2 = botTarId[botId][2];	// botIdsell目标id
	int goodId = wbArr[botIdid1].wbType;	// buy的货物种类

	if (botTarId[botId][0] == 1) {		// 如果此时机器人botId当前奔向的是buy目标
		// 这里不需要检查productNeed，因为botId上一次更新时，已经知道了goodId被需要了

		bool robFlag = false;	// 标识抢别人的目标是否抢成功，只抢别的机器人的buy端
		double botIdLastWeight = getWeight(botId, botIdid1, botIdid2, goodId);  // 未更新目标时的权值
		double botIdCurWeight = botIdLastWeight;
		int newTarId = findBestWbBuy(botId, goodId, botIdCurWeight);
		//if (newTarId != -1 && (botIdid1 != newTarId)) {	// 看能否找到新的buy目标，且不会自己找自己

		//	for (int i = 0; i < 4; ++i) {
		//		if (newTarId == botTarId[i][1]) {	// 看看这个buy的目标有没有被别人锁定（只可能被一个机器人锁定，且不会是自己）
		//			int wbId1 = -1, wbId2 = -1;
		//			double iLastWeight = getWeight(i, botTarId[i][1], botTarId[i][2], goodId);	// 此时机器人i肯定没有到达newTarId，因为，它一旦到达，就会释放这个buy目标
		//			double iCurWeight = iLastWeight;

		//			findNext2(i, wbId1, wbId2, iCurWeight);	// 肯定不会自己找自己的目标，因为自己已经把自己的目标锁定住了

		//			if (wbId1 != -1 && wbId2 != -1 && (botIdCurWeight - botIdLastWeight) > penalty1 * (iLastWeight - iCurWeight)) {	// 权值增益大于权值衰减

		//				robFlag = true;

		//				int iId1 = botTarId[i][1];	// 机器人i的buy工作台
		//				int iId2 = botTarId[i][2];	// 机器人i的sell工作台

		//				// 释放原来机器人botId的buy工作台的输出端，和i的buy工作台的输出端和sell工作台的输入端
		//				wbIfTar[botIdid1][1] = 0;								// 释放工作台botIdid1的输出端
		//				if (wbArr[iId2].wbType != 8 && wbArr[iId2].wbType != 9)
		//					wbIfTar[iId2][0] ^= (1 << goodId);					// 释放工作台iId2的输入端
		//				wbIfTar[iId1][1] = 0;									// 释放工作台iId1的输出端

		//				// 更新机器人的目标工作台
		//				botTarId[botId] = { 1,newTarId,botIdid2 };
		//				botTarId[i] = { 1,wbId1,wbId2 };

		//				// 锁定新的机器人botId的buy工作台的输出端，和i的buy工作台的输出端和sell工作台的输入端
		//				wbIfTar[newTarId][1] = 1;								// 锁定工作台newTarId的输出端
		//				if (wbArr[wbId2].wbType != 8 && wbArr[wbId2].wbType != 9)
		//					wbIfTar[wbId2][0] = (wbIfTar[wbId2][0] | (1 << wbArr[wbId1].wbType));	// 锁定工作台wbId2的输入端
		//				wbIfTar[wbId1][1] = 1;									// 锁定工作台wbId1的输出端

		//				// 更新机器人移动的目标
		//				botArr[botId].setMoveTarget(wbArr[botTarId[botId][1]]);
		//				botArr[botId].moveUntillArriveTarget_YNG4();
		//				botArr[i].setMoveTarget(wbArr[botTarId[i][1]]);
		//				botArr[i].moveUntillArriveTarget_YNG4();
		//			}
		//			break;
		//		}
		//	}
		//}		

		if (robFlag == false) {		// 如果抢别人的目标不成功，则试图找没有机器人锁定的buy工作台，且更新后能小赚

			double x = botArr[botId].x, y = botArr[botId].y;
			double minDist = 1000.0;
			int tarId = -1;
			for (int i = 0; i < wbTypeArr[goodId].size(); ++i) {
				int id = wbTypeArr[goodId][i];
				if (wbArr[id].ifHaveProduct && !wbIfTar[id][1]) {    // 有货物，且未被锁定（自己的目标已经被自己锁定了），才有机会被选择
					double dist = calDist(x, y, wbArr[id].x, wbArr[id].y);
					if (dist < minDist) {
						tarId = id;
						minDist = dist;
					}
				}
			}
			if (tarId != -1) {

				double botIdCurWeight = getWeight(botId, tarId, botIdid2, goodId);
				if (botIdCurWeight <= penalty2 * botIdLastWeight)
					return;

				// 释放原来的工作台的输出端
				wbIfTar[botIdid1][1] = 0;
				// 锁定新目标工作台的输出端
				wbIfTar[tarId][1] = 1;

				// 更新机器人的目标工作台
				botTarId[botId] = { 1,tarId,botIdid2 };

				// 更新机器人移动的目标
				botArr[botId].setMoveTarget(wbArr[botTarId[botId][1]]);
				botArr[botId].moveUntillArriveTarget_YNG4();
			}
		}
	}

	else if (botTarId[botId][0] == 2) {	// 如果此时机器人botId当前奔向的是sell目标

		double botIdLastWeight = getWeight2(botId, botIdid2, goodId);  // 未更新目标时的权值
		double botIdCurWeight = botIdLastWeight;
		int tarId = -1;
		findNext3(botId, goodId, tarId, botIdCurWeight);	// 肯定不会自己找自己的目标，因为自己已经把自己的目标锁定住了
		if (tarId != -1) {

			if (botIdCurWeight <= penalty3 * botIdLastWeight)
				return;

			// 释放原来的工作台的输入端
			wbIfTar[botIdid2][0] ^= (1 << goodId);
			// 锁定新目标工作台的输入端
			if (wbArr[tarId].wbType != 8 && wbArr[tarId].wbType != 9)
				wbIfTar[tarId][0] = (wbIfTar[tarId][0] | (1 << goodId));
			botTarId[botId][2] = tarId;
			// 更新机器人移动的目标
			botArr[botId].setMoveTarget(wbArr[tarId]);
			botArr[botId].moveUntillArriveTarget_YNG4();

		}
	}

}
