function checkEmptPetNum(charaindex)
	EmptyPetNum = 0;
	for i = 0, 4 do
		if char.check(char.getCharPet(charaindex, i)) == 0 then
			EmptyPetNum = EmptyPetNum + 1;
		end
	end
	return EmptyPetNum;
end

function checkEmptItemNum(charaindex)
	EmptyItemNum = 0;
	for i = 9, 23 do
		if char.getItemIndex(charaindex, i) == -1 then
			EmptyItemNum = EmptyItemNum + 1;
		end
	end
	return EmptyItemNum;
end

function BattleOver(meindex, battleindex, iswin)
	for i=0, 4 do
		charaindex = battle.getCharOne(battleindex, i, 0);
		if char.check(charaindex) == 1 then
			local npcid = char.getWorkInt(meindex, "临时整数1");
			char.setInt(charaindex, "楼层时间", 20 * 60);
			if char.getFlg(charaindex, "死亡") == 1 then
				char.DischargeParty(charaindex, 1);
				char.WarpToSpecificPoint(charaindex, npcmapdata[1], npcmapdata[2], npcmapdata[3]);
			end
			if iswin == 1 then
				char.setWorkInt(charaindex, "临时整数1", npcid);
				char.TalkToCli(charaindex, meindex, "你们好利害，竟然把我给打败了！", "黄色");
			end
		end
	end
end


--num = math.floor(math.random(1, 10000));
--		NLG.ShowWindowTalked(_PlayerIndex, 1, 3, 1, "               魔王验证码:["..num.."] \n\n    欢迎来挑战魔王！成功挑战魔王你将获得道具VIP宠物和道具哦！前提是必须要有门票哦！才可以进入哦\n\n请输入最上面的验证码：\n\n", _MeIndex); 
--end

function question(meindex, talkerindex, seqno, token)
	num = char.setWorkInt(talkerindex, "计时器", math.random(8999)+1000);
 -- num = math.floor(math.random(1, 3));
	local floorid = char.getWorkInt(meindex, "临时整数1");
	local token = "";
	if floorid > 0 then
		token = "    欢迎来挑战魔王！"
				.."\n成功挑战魔王你将获得宠物和道具哦！"
				.."\n前提是必须要有足够的体力哦！"
				.."\n挑战本楼需要[style c=4]"..npctl[floorid].."[/style]体力"
				.."\n请输入最下面的验证码："
				.."\n\n请输入魔王验证码:『[style c=5]"..num.."[/style]』\n\n";
	else
		token = "    欢迎来挑战魔王！"
				.."\n成功挑战魔王你将获得宠物和道具哦！"
				.."\n前提是必须要有足够的体力哦！"
				.."\n请输入最下面的验证码："
				.."\n\n请输入魔王验证码:『[style c=5]"..num.."[/style]』\n\n";
	end
	lssproto.windows(talkerindex, "输入框", 12, seqno, char.getWorkInt( meindex, "对象"), token, char.getWorkInt(talkerindex, "计时器"));
end


--NPC对话事件(NPC索引)
function Talked(meindex, talkerindex , szMes, color )
	if npc.isFaceToFace(meindex, talkerindex) == 1 then 
		if char.getWorkInt(meindex, "临时整数1") == 0 then
			token = "3                 『" .. char.getChar(meindex, "名字") .. "』\n"
						 .. "\n想要接受阎王挑战吗??"
						 .. "\n                   我要接受挑战"
						 .. "\n                   我要进行观战"
						 .. "\n                   查看进入时间"
						 .. "\n                   缩短进入时间";
			lssproto.windows(talkerindex, "选择框", "取消", 1, char.getWorkInt( meindex, "对象"), token);
		else
			if char.getWorkInt(talkerindex, "临时整数1") ~= char.getWorkInt(meindex, "临时整数1") then
				token = "想要接受该楼层的死亡挑战吗??" ;
				question(meindex, talkerindex, 0, token);
			else
				token = "4                 『" .. char.getChar(meindex, "名字") .. "』\n\n"
							 .. "你太利害了，我已经被你打败了，我认输了\n"
							 .. "\n             继续前进到下一关卡\n"
							 .. "\n             领取奖励关退出挑战";
				lssproto.windows(talkerindex, "选择框", "取消", 0, char.getWorkInt( meindex, "对象"), token);
			end
		end
	end
end

--NPC窗口事件(NPC索引)
function WindowTalked ( meindex, talkerindex, _seqno, _select, _data)
	if _select == 8 then
		return;
	end
	if npc.isFaceToFace(meindex, talkerindex) == 1 then 
		if _seqno == 0 then
			if char.getWorkInt(talkerindex, "临时整数1") ~= char.getWorkInt(meindex, "临时整数1") then
				
		
					id = char.getWorkInt(meindex, "临时整数1");
					if id < 1 or id > 20 then
						char.TalkToCli(talkerindex, -1, "系统出问题了,请于管理员联系", "红色");
						return;
					end
		
					if char.getWorkInt(talkerindex, "组队") == 2 then
						return;
					end
	
					battleindex = battle.CreateVsEnemy(talkerindex, meindex, enemytable[id]);
					char.setWorkInt(meindex,"禁止求救",1);
					char.setWorkInt(talkerindex,"禁止求救",1);
					char.setWorkInt(meindex,"禁止逃跑",1);
					char.setWorkInt(talkerindex,"禁止逃跑",1);
					char.TalkToCli(talkerindex, meindex, "妈B，挑战哥，你不想活了，日死你！", "黄色");
			
					char.setWorkInt(talkerindex, "计时器", math.random(8999)+1000);
			
			else
				local num = other.atoi(_data)
				if num == 1 then
					token = "想要进入下一层吗?请回答以下验证码??";
					question(meindex, talkerindex, 10, token);
				elseif num == 3 then
					token = "想要领取奖励吗?请回答以下验证码??";
					question(meindex, talkerindex, 11, token);
				end
			end
		elseif _seqno == 1 then
			if _select == 0 then
				local num = other.atoi(_data);
				if num == 1 then
					token = "想要进入楼层挑战吗?请回答以下验证码??";
					question(meindex, talkerindex, 12, token);
				elseif num == 2 then
					token = "                 『" .. char.getChar(meindex, "名字") .. "』\n\n"
							 .. "请输入你想要观战的楼层，如果观战1楼请输入1，如果观战2楼请输入2";
					lssproto.windows(talkerindex, "输入框", "确定|取消", 2, char.getWorkInt( meindex, "对象"), token);
				elseif num == 3 then
					if char.getInt(talkerindex, "楼层时间") > 0 then
						char.TalkToCli(talkerindex, meindex, "请在" .. char.getInt(talkerindex, "楼层时间") .. "秒后再来挑战！", "黄色");
					else
						char.TalkToCli(talkerindex, meindex, "你现在已经可以进入挑战了！", "黄色");
					end
				elseif num == 4 then
					if char.getInt(talkerindex, "楼层时间") > 0 then
						token = "                 『" .. char.getChar(meindex, "名字") .. "』\n\n"
								 .. "您想要缩短进入等待时间吗？每缩等1秒你需给我支付1声望哟！你当前等待时间是" .. char.getInt(talkerindex, "楼层时间") .. "秒，请问你需要短缩多少秒？？";
						lssproto.windows(talkerindex, "输入框", "确定|取消", 3, char.getWorkInt( meindex, "对象"), token);
					else
						char.TalkToCli(talkerindex, meindex, "当前你无需缩短时间即可进入！", "黄色");
					end
				end
			end
		elseif _seqno == 2 then
			if _select == 1 then
				local num = other.atoi(_data);
				if num < 1 then
					num = 1;
				elseif num > 20 then
					num = 20;
				end
				if char.check(npcindex[num]) == 1 then
					if char.getWorkInt(npcindex[num], "战斗") ~= 0 then
						battle.WatchEntry(talkerindex, npcindex[num]);
						return;
					end
				end
				char.TalkToCli(talkerindex, meindex, num .. "楼并没有战斗！", "黄色");
			end
		elseif _seqno == 3 then
			if _select == 1 then
				local num = other.atoi(_data);
				if num > 0 then
					if num > char.getInt(talkerindex, "楼层时间") then
						num = math.max(1, char.getInt(talkerindex, "楼层时间"));
					end
					if char.getInt(talkerindex, "声望") < num * 100 then
						char.TalkToCli(talkerindex, meindex, "你当前声望数不足以缩短" .. num .. "秒", "黄色");
					else
						char.setInt(talkerindex, "楼层时间", char.getInt(talkerindex, "楼层时间") - num);
						char.setInt(talkerindex, "声望", char.getInt(talkerindex, "声望") - num * 100);
						char.TalkToCli(talkerindex, meindex, "你当前已缩短" .. num .. "秒，扣除" .. num .. "声望", "黄色");
					end
				else
					char.TalkToCli(talkerindex, meindex, "请正常输入短缩的时间！", "黄色");
				end
			end
		elseif _seqno == 10 then 
			if other.atoi(_data) == char.getWorkInt(talkerindex, "计时器") then
				if char.getWorkInt(talkerindex, "组队") ~= 0 then
					char.TalkToCli(talkerindex, meindex, "请解散团队，我才能为你传送上去！", "黄色");
					return
				end
		
				local npcid = char.getWorkInt(meindex, "临时整数1");
				
				if npcid == maxfloord then
					char.TalkToCli(talkerindex, meindex, "这里已经是最后一关了，你直接领奖品吧！", "黄色");
					return
				end
				char.WarpToSpecificPoint(talkerindex, 40001+npcid, 5, 6);
				
			end
		elseif _seqno == 11 then
			if other.atoi(_data) == char.getWorkInt(talkerindex, "计时器") then
				if checkEmptItemNum(talkerindex) == 0 then
					char.TalkToCli(talkerindex, meindex, "物品已满，请道具栏留有足够的空位！", "黄色");
					return;
				end
				local npcid = char.getWorkInt(meindex, "临时整数1");
				local rnd = math.random(100);
				if npcid == 1 then
					jl = 4;
				elseif npcid == 2 then
					jl = 8;
				elseif npcid == 3 then
					jl = 12;
				elseif npcid == 4 then
					jl = 20;
				end
				if rnd <= jl then
					itemidmin = awardlist[npcid][1][1];
					itemidmax = awardlist[npcid][1][2];
					itemid = math.random(itemidmin, itemidmax);
					npc.AddItem(talkerindex, itemid)
					char.talkToAllServer("[阎王挑战快讯]恭喜勇者 " .. char.getChar(talkerindex, "名字")  .. " 在阎王挑战第" .. npcid .. "关中获得"  .. item.getNameFromNumber(itemid),4);
				elseif rnd <= 70 + jl then
					char.setInt(talkerindex, "刷楼积分", char.getInt(talkerindex, "刷楼积分") + npcid*2);
					char.talkToAllServer("[楼层挑战快讯]恭喜勇者 " .. char.getChar(talkerindex, "名字")  .. " 在楼层挑战第" .. npcid .. "关获得".. npcid*2 .."刷楼积分",4);
				else
					char.setInt(talkerindex, "活力", char.getInt(talkerindex, "活力") + npcid * 10);
					char.talkToAllServer("[阎王挑战快讯]恭喜勇者 " .. char.getChar(talkerindex, "名字")  .. " 在阎王挑战第" .. npcid .. "关获得" .. npcid * 10 .. "点活力!",4);
				end
				char.WarpToSpecificPoint(talkerindex, npcmapdata[1], npcmapdata[2],npcmapdata[3]);
			end
		elseif _seqno == 12 then
			if other.atoi(_data) == char.getWorkInt(talkerindex, "计时器") then
				if char.getWorkInt(talkerindex, "组队") ~= 0 then
					char.TalkToCli(talkerindex, -1, "请解散团队后进入！", "黄色");
					return;
				end

				if char.getInt(talkerindex, "楼层时间") > 0 then
					char.TalkToCli(talkerindex, meindex, "请在" .. char.getInt(talkerindex, "楼层时间") .. "秒后再来挑战！", "黄色");
					return;
				end
				
				--if npc.Free(meindex, talkerindex, "ITEM=25500") == 1 then
					--npc.DelItem(talkerindex, "25500*1");
				--else
					--char.TalkToCli(talkerindex, meindex, "你身上并没有阎王挑战门票，无法给你进入!", "黄色");
					--return;
				--end
				
				char.setInt(talkerindex, "楼层时间", 20 * 60);
				
				char.talkToAllServer( "[阎王挑战快讯]勇者 " .. char.getChar(talkerindex, "名字") .. " 参加阎王挑战，你也快来吧！", "青色");
				char.WarpToSpecificPoint(talkerindex, 40001, 5, 7);
				char.setWorkInt(talkerindex, "临时整数1", 0);
			end
		end
	end
end

function getIntPart(x)
    if x <= 0 then
       return math.ceil(x);
    end

    if math.ceil(x) == x then
       x = math.ceil(x);
    else
       x = math.ceil(x) - 1;
    end
    return x;
end

function mydata()
	awardtype = {"宠物", "物品"};

	awardlist = {{{25501, 25596}, {24333, 24333}, {24157, 24158}, {24151, 24156}}
				,{{25501, 25596}, {24333, 24333}, {24157, 24158}, {24151, 24153}}
				,{{25501, 25596}, {24333, 24333}, {24157, 24158}, {24151, 24156}}
				,{{25501, 25596}, {24333, 24333}, {24157, 24158}, {24151, 24153}}
				};
	
	changeaward = {{2, 25802, {{25501, 25502, 25503, 25504}, 3000000, 15000}} --白虎
				  ,{2, 25801, {{25505, 25506, 25507, 25508}, 3500000, 15000}} --水暴
				  ,{2, 25803, {{25509, 25510, 25511, 25512}, 2000000, 10000}} --乌力王
				  ,{2, 25804, {{25513, 25514, 25515, 25516}, 3000000, 15000}} --青龙
				  ,{2, 25805, {{25517, 25518, 25519, 25520}, 4000000, 20000}} --猴子
				  ,{2, 25806, {{25521, 25522, 25523, 25524}, 3000000, 10000}} --无魔
				  ,{2, 25807, {{25525, 25526, 25527, 25528}, 2000000, 10000}} --狂牛
				  ,{2, 25808, {{25529, 25530, 25531, 25532}, 2000000, 10000}} --麋鹿
				  ,{2, 25809, {{25533, 25534, 25535, 25536}, 2000000, 10000}} --蜜蜂
				  ,{2, 25810, {{25537, 25538, 25539, 25540}, 4000000, 20000}} --牛人
				  ,{2, 25811, {{25541, 25542, 25543, 25544}, 3000000, 15000}} --猫人
				  ,{2, 25812, {{25545, 25546, 25547, 25548}, 4000000, 20000}} --鸡年
				  ,{2, 25813, {{25549, 25550, 25551, 25552}, 2000000, 10000}} --甲虫
				  ,{2, 25814, {{25553, 25554, 25555, 25556}, 3000000, 15000}} --朱雀
				  ,{2, 25815, {{25557, 25558, 25559, 25560}, 3000000, 15000}} --玄武
				  ,{2, 25816, {{25561, 25562, 25563, 25564}, 2000000, 10000}} --龙蜥
				  ,{2, 25817, {{25565, 25566, 25567, 25568}, 2000000, 10000}} --双狼
				  ,{2, 25818, {{25569, 25570, 25571, 25572}, 2000000, 10000}} --鹰人
				  --,{1, 2565, {{25573, 25574, 25575, 25576}, 3000000, 15000}} --白狼
				  ,{2, 25819, {{25573, 25574, 25575, 25576}, 3000000, 15000}} --白狼
				  ,{2, 25832, {{25577, 25578, 25579, 25580}, 4000000, 15000}} --蝴蝶
				  ,{2, 25833, {{25581, 25582, 25583, 25584}, 4000000, 20000}} --虎鲸
				  ,{2, 25834, {{25585, 25586, 25587, 25588}, 4000000, 20000}} --鳄人
				  ,{2, 25837, {{25589, 25590, 25591, 25592}, 1000000, 2000}}  --狒狒
				  ,{2, 25838, {{25593, 25594, 25595, 25596}, 3000000, 15000}} --狗年
				  ,{2, 26017, {{25597, 25598, 25599, 25600}, 6000000, 15000}} --白虎骑证
				  ,{2, 26019, {{25601, 25602, 25603, 25604}, 6000000, 15000}} --金暴骑证
				  ,{2, 26020, {{25605, 25606, 25607, 25608}, 6000000, 15000}} --黑暴骑证
				  ,{2, 26021, {{25609, 25610, 25611, 25612}, 6000000, 15000}} --蓝虎骑证
				  };
	maxpage = getIntPart(table.getn(changeaward) / 6) + 1;
	enemytable = {{1, 2, 3, 4, -1, -1, -1, -1, -1, -1}
				 ,{2956, 2963, 2964, 2965, 2966, -1, -1, -1, -1, -1}
				 ,{2957, 2967, 2968, 2969, 2970, -1, -1, -1, -1, -1}
				 ,{2958, 2971, 2972, 2973, 2974, -1, -1, -1, -1, -1}
				 };
	npcindex = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
	maxfloord = 7;--开放的总层数
	npcdata = {{"爱玩阎罗殿",101996,777,47,45,6,0}
			  ,{"阎罗殿1F",100000,40001,5,5,4,1}
			  ,{"阎罗殿2F",100000,40002,5,5,4,2}
			  ,{"阎罗殿3F",100000,40003,5,5,4,3}
			  ,{"阎罗殿4F",100000,40004,5,5,4,4}
			  ,{"阎罗殿5F",100000,40005,5,5,4,5}
			  ,{"阎罗殿6F",100000,40006,5,5,4,6}
			  ,{"阎罗殿7F",100000,40007,5,5,4,7}
			  ,{"阎罗殿8F",100000,40008,5,5,4,8}
			  ,{"阎罗殿9F",100000,40009,5,5,4,9}
			  ,{"阎罗殿10F",100000,40010,5,5,4,10}
			  };
	npcmapdata = {2005,13,13};
	npctl = {2,2,2,5,5,8,8,10,10,10};
end

function Create(name, metamo, floor, x, y, dir, flg, lv)
	local index = npc.CreateNpc(name, metamo, floor, x, y, dir);
	char.setFunctionPointer(index, "对话事件", "Talked", "");
	char.setFunctionPointer(index, "窗口事件", "WindowTalked", "");
	char.setFunctionPointer(index, "战后事件", "BattleOver", "");
	char.setInt(index, "等级", lv);
	char.setWorkInt(index, "临时整数1", flg);
	if flg > 0 and flg <= table.getn(npcindex) then
		npcindex[flg] = index;
	end
end

function reloadnpc()
	mydata();
	for i=1,table.getn(npcindex) do
		if npcindex[i] >= 0 then
			print("重读NPC信息",char.getChar(npcindex[i], "名字"));
			char.setChar(npcindex[i], "名字",npcdata[i][1]);
			char.setInt(npcindex[i], "图像号", npcdata[i][2]);
			char.setInt(npcindex[i], "原图像号", npcdata[i][2]);
			char.WarpToSpecificPoint(npcindex[i], npcdata[i][3], npcdata[i][4], npcdata[i][5]);
			char.setInt(npcindex[i], "方向", npcdata[i][6]);
			char.ToAroundChar(npcindex[i]);
		end
	end
	
end

function main()
	mydata();
	--Create(npcdata[1][1], npcdata[1][2], npcdata[1][3], npcdata[1][4], npcdata[1][5], npcdata[1][6], npcdata[1][7], 140);
	--Create(npcdata[2][1], npcdata[2][2], npcdata[2][3], npcdata[2][4], npcdata[2][5], npcdata[2][6], npcdata[2][7], 140);
	--Create(npcdata[3][1], npcdata[3][2], npcdata[3][3], npcdata[3][4], npcdata[3][5], npcdata[3][6], npcdata[3][7], 140);
	--Create(npcdata[4][1], npcdata[4][2], npcdata[4][3], npcdata[4][4], npcdata[4][5], npcdata[4][6], npcdata[4][7], 140);
	--Create(npcdata[5][1], npcdata[5][2], npcdata[5][3], npcdata[5][4], npcdata[5][5], npcdata[5][6], npcdata[5][7], 140);
	--Create(npcdata[6][1], npcdata[6][2], npcdata[6][3], npcdata[6][4], npcdata[6][5], npcdata[6][6], npcdata[6][7], 140);
	--Create(npcdata[7][1], npcdata[7][2], npcdata[7][3], npcdata[7][4], npcdata[7][5], npcdata[7][6], npcdata[7][7], 140);
	--Create(npcdata[8][1], npcdata[8][2], npcdata[8][3], npcdata[8][4], npcdata[8][5], npcdata[8][6], npcdata[8][7], 140);
	--Create(npcdata[9][1], npcdata[9][2], npcdata[9][3], npcdata[9][4], npcdata[9][5], npcdata[9][6], npcdata[9][7], 140);
	--Create(npcdata[10][1], npcdata[10][2], npcdata[10][3], npcdata[10][4], npcdata[10][5], npcdata[10][6], npcdata[10][7], 140);
	--Create(npcdata[11][1], npcdata[11][2], npcdata[11][3], npcdata[11][4], npcdata[11][5], npcdata[11][6], npcdata[11][7], 140);
	
	magic.addLUAListFunction("重读刷楼系统", "reloadnpc", "", 1, "[重读刷楼系统]");
end
