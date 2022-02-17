function printf(talkerindex, str)
	char.TalkToCli(talkerindex, -1, str, 6);
end

function StringSplit(_data, _splitint)-- 分割处理函数（返回数组）
	local StartIndex = 1;
	local SplitIndex = 1;
	local SplitArray = {};
	while true do
		local FindLastIndex = string.find(_data, _splitint, StartIndex);
		if not FindLastIndex then
			SplitArray[SplitIndex] = string.sub(_data, StartIndex, string.len(_data));
			break ;
		end
		SplitArray[SplitIndex] = string.sub(_data, StartIndex, FindLastIndex - 1);
		StartIndex = FindLastIndex + string.len(_splitint);
		SplitIndex = SplitIndex + 1;
	end
	return SplitArray;
end

function FreeTitleShow(charaindex,_num)--进入查询
	--发送当前选择的称号信息
	local restr = _num.."\n";--返回文本
	local maxnum = _num * 10 - 10;
	--1称号编号|2称号名字|3称号形象|4称号血|5称号攻|6称号防|7称号敏|8称号时间
	local cdkey = char.getChar(charaindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(charaindex,"存档");--取得玩家序号
	for i=1,10 do
		local myTitlesql = string.format( "Data%02d",maxnum+i)..","..string.format( "Time%02d",maxnum+i);
		local token = "select ".. myTitlesql .." from `CSA_Title` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
		local ret = sasql.query(token);
		if ret == 1 then
			sasql.free_result();
			sasql.store_result();
			local num = sasql.num_rows();
			if num > 0 then--如果查询信息存在的话，获取信息
				sasql.fetch_row();
				local myTitleid = other.atoi(sasql.data(1));--取得称号编号
				local myTitletime = other.atoi(sasql.data(2));--取得称号时间
				if myTitleid >= 0 then
					token = "SELECT * FROM Data_Title WHERE ID="..myTitleid..";";--从称号编号获取称号数据
					local ret2 = sasql.query(token);
					if ret2 == 1 then
						sasql.free_result();
						sasql.store_result();
						local num2 = sasql.num_rows();
						if num2 > 0 then
							sasql.fetch_row();
							local Titlename = sasql.data(2);--取得称号名字
							local Titlehp = sasql.data(3);--取得称号血加成
							local Titlestr = sasql.data(4);--取得称号攻加成
							local Titletgh = sasql.data(5);--取得称号防加成
							local Titledex = sasql.data(6);--取得称号敏加成
							local Titlemo = sasql.data(7);--取得称号图号
							--1称号编号|2称号名字|3称号图像|4称号加血|5称号加攻|6称号加防|7称号加敏|8称号时间
							restr = restr .. (maxnum+i) .."|"..Titlename.."|"..Titlemo.."|"..Titlehp.."|"..Titlestr.."|"..Titletgh.."|"..Titledex.."|"..myTitletime.."\n";
						else
							restr = restr .. (maxnum+i).."||0|0|0|0|0|0|-1\n";
						end
					else
						restr = restr .. (maxnum+i).."||0|0|0|0|0|0|-1\n";
					end
				else
					restr = restr .. (maxnum+i).."||0|0|0|0|0|0|-1\n";
				end
			else
				restr = restr .. (maxnum+i).."||0|0|0|0|0|0|-1\n";
			end
		else
			restr = restr .. (maxnum+i).."||0|0|0|0|0|0|-1\n";
		end
	end
	--print(restr)
	lssproto.windows(charaindex, 1011, 12, 0, char.getWorkInt( npcindex, "对象"), restr);
end

function InitPlayerTitle(charaindex)--检查玩家称号状态
	local cdkey = char.getChar(charaindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(charaindex,"存档");--取得玩家序号
	local token = "select * from `CSA_Title` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
	local ret = sasql.query(token);
	local mynum = 0;
	local mytime = 0;
	local myfmid = -1;
	local myfmmap = -1;
	if ret == 1 then
		sasql.free_result();
		sasql.store_result();
		local num = sasql.num_rows();
		if num == 0 then
			--还没有创建称号数据，进入创建
			char.TalkToCli(charaindex, -1, "首次使用称号系统，正在为您初始化称号数据...", 6);
			token = "INSERT INTO `CSA_Title` (`Account` ,`SaveIndex`) VALUES ('" .. cdkey.. "',"..Saveindex.. ")";
			sasql.query(token);
			char.TalkToCli(charaindex, -1, "初始化完成，请重新使用称号功能！", 6);
			return;
		end
		sasql.fetch_row();
		local mytime = os.time();
		local delTitle = {};
		for i=1,50 do--统计到期称号
			local Titletime = other.atoi(sasql.data(53+i));
			local Titleid = other.atoi(sasql.data(3+i));
			if Titletime > 0 then
				if Titletime < mytime then
					table.insert(delTitle,i);
					--移除称号效果
					char.setInt(charaindex,"现称号属性",0);
					char.setInt(charaindex,"现称号图像",0);
					char.complianceParameter(charaindex);
					char.ToAroundChar(charaindex);
					--char.sendArroundCharaData(charaindex);--更新特效
					char.WarpToSpecificPoint(charaindex, char.getInt(charaindex, "地图号"), char.getInt(charaindex, "坐标X"), char.getInt(charaindex, "坐标Y"));
					char.TalkToCli(charaindex, -1, "您第"..i.."号称号已到期，系统自动清除！", 6);
				end
			end
		end
		--开始删除
		token = "update `CSA_Title` SET ";
		for i=1,table.getn(delTitle) do
			if i >= table.getn(delTitle) then
				token = token .. string.format( "Data%02d=0,Time%02d=0",delTitle[i],delTitle[i]);
			else
				token = token .. string.format( "Data%02d=0,Time%02d=0,",delTitle[i],delTitle[i]);
			end
		end
		token = token .. " where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex;
		sasql.query(token);
	else
		char.TalkToCli(charaindex, -1, "数据获取失败，请重新尝试，如多次无法获取数据，请联系管理员查看！", 6);
		return;
	end
	
	
end

function FreeTitle(charaindex)
	InitPlayerTitle(charaindex);
	FreeTitleShow(charaindex,1);
end

function WindowTalked ( _meindex, _talkerindex, _seqno, _select, _data)
	if _seqno == 0 then
		if (_select==16) then--上一页
			local Titlepage = other.atoi(_data)-1;
			if Titlepage < 1 then
				Titlepage = 5;
			end
			FreeTitleShow(_talkerindex,Titlepage);
		elseif (_select==32) then--上一页
			local Titlepage = other.atoi(_data)+1;
			if Titlepage > 5 then
				Titlepage = 1;
			end
			FreeTitleShow(_talkerindex,Titlepage);
		else
			local Titledata = StringSplit(_data, "|");--分割客户端传递信息
			if table.getn(Titledata) < 2 then
				return;
			end
			if Titledata[1] == "显示称号" then
				local Titleid = other.atoi(Titledata[2]);
				local cdkey = char.getChar(_talkerindex,"帐号");--取得玩家帐号
				local Saveindex = char.getInt(_talkerindex,"存档");--取得玩家序号
				local myTitlesql = string.format( "Data%02d",Titleid)..","..string.format( "Time%02d",Titleid);
				local token = "select "..myTitlesql.." from `CSA_Title` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
				local ret = sasql.query(token);
				local mynum = 0;
				local mytime = 0;
				local myfmid = -1;
				local myfmmap = -1;
				if ret == 1 then
					sasql.free_result();
					sasql.store_result();
					local num = sasql.num_rows();
					if num > 0 then
						sasql.fetch_row();
						local myTitleid = other.atoi(sasql.data(1));--取得称号编号
						local myTitletime = other.atoi(sasql.data(2));--取得称号时间
						if myTitletime > 0 then
							if myTitletime < os.time() then--称号到期，删除称号
								char.TalkToCli(_talkerindex, -1, "该称号已经到期，无法使用!", 6);
								return ;
							end
						end
						if myTitleid <= 0 then
							char.TalkToCli(_talkerindex, -1, "？？？", 6);
							return ;
						end
						--取得称号加成数据
						local token2 = "SELECT * FROM Data_Title WHERE ID="..myTitleid..";";--从称号编号获取称号数据
						local ret2 = sasql.query(token2);
						if ret2 == 1 then
							sasql.free_result();
							sasql.store_result();
							local num2 = sasql.num_rows();
							if num2 <= 0 then
								char.TalkToCli(_talkerindex, -1, "该称号数据未存在！", 6);
								return;
							end
							sasql.fetch_row();
							local Titlename = sasql.data(2);--取得称号名字
							local Titlehp = sasql.data(3);--取得称号血加成
							local Titlestr = sasql.data(4);--取得称号攻加成
							local Titletgh = sasql.data(5);--取得称号防加成
							local Titledex = sasql.data(6);--取得称号敏加成
							local Titlemo = sasql.data(7);--取得称号图号
							--print(Titlemo);
							local Titlejc = char.getLiftTo8(Titlehp, 1) + char.getLiftTo8(Titlestr, 2) + char.getLiftTo8(Titletgh, 3) + char.getLiftTo8(Titledex, 4);
							char.setInt(_talkerindex,"现称号属性",Titlejc);
							char.setInt(_talkerindex,"现称号图像",Titlemo);
							char.complianceParameter(_talkerindex);
							char.Updata(_talkerindex,29158);
							char.ToAroundChar(_talkerindex);
							--char.sendArroundCharaData(_talkerindex);--更新特效
							char.WarpToSpecificPoint(_talkerindex, char.getInt(_talkerindex, "地图号"), char.getInt(_talkerindex, "坐标X"), char.getInt(_talkerindex, "坐标Y"));
				
						end
					end
				else
					char.TalkToCli(_talkerindex, -1, "数据获取失败，请重新尝试，如多次无法获取数据，请联系管理员查看！", 6);
					return;
				end
			elseif Titledata[1] == "取消显示" then
				--移除称号效果
				local Titlejc = char.getLiftTo8(0, 1) + char.getLiftTo8(0, 2) + char.getLiftTo8(0, 3) + char.getLiftTo8(0, 4);
				char.setInt(_talkerindex,"现称号属性",0);
				char.setInt(_talkerindex,"现称号图像",0);
				char.complianceParameter(_talkerindex);
				char.ToAroundChar(_talkerindex);
				char.Updata(_talkerindex,29158);
				--char.sendArroundCharaData(_talkerindex);--更新特效
				char.WarpToSpecificPoint(_talkerindex, char.getInt(_talkerindex, "地图号"), char.getInt(_talkerindex, "坐标X"), char.getInt(_talkerindex, "坐标Y"));
				char.TalkToCli(_talkerindex, -1, "已取消称号显示！", 4);
			end
		end
	end
end

function SetTitle(itemindex, charaindex, toindex, haveitemindex)
	local itemdata = StringSplit(item.getChar(itemindex, "字段"),"|");
	--1称号编号,2称号名字,3称号时间
	if table.getn(itemdata) < 3 then
		char.TalkToCli(_talkerindex, -1, "该道具有问题，无法使用！", 4);
		return ;
	end
	local cdkey = char.getChar(charaindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(charaindex,"存档");--取得玩家序号
	local token = "select * from `CSA_Title` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
	local ret = sasql.query(token);
	local mynum = 0;
	local mytime = 0;
	local myfmid = -1;
	local myfmmap = -1;
	if ret == 1 then
		sasql.free_result();
		sasql.store_result();
		local num = sasql.num_rows();
		if num == 0 then
			--还没有创建称号数据，进入创建
			char.TalkToCli(charaindex, -1, "首次使用称号系统，正在为您初始化称号数据...", 6);
			token = "INSERT INTO `CSA_Title` (`Account` ,`SaveIndex`) VALUES ('" .. cdkey.. "',"..Saveindex.. ")";
			sasql.query(token);
			char.TalkToCli(charaindex, -1, "初始化完成，请重新使用称号道具！", 6);
			return;
		end
		sasql.fetch_row();
		local mytime = os.time();
		local delTitle = {};
		local settime = 0;
		if other.atoi(itemdata[3]) > 0 then
			settime = os.time()+(other.atoi(itemdata[3])*60*60*24);
		else
			settime = 0;
		end
		-------------------- 判断称号是否存在 --------------------
		for i=1,50 do--统计到期称号
			local Titleid = other.atoi(sasql.data(3+i));
			local TitleTime = other.atoi(sasql.data(53+i));
			if Titleid == other.atoi(itemdata[1]) then
				if TitleTime > 0 then
					if other.atoi(itemdata[3]) > 0 then
						settime = TitleTime+(other.atoi(itemdata[3])*60*60*24);
						if settime < mytime then
							settime = os.time()+(other.atoi(itemdata[3])*60*60*24);
						end
					else
						settime = 0;
					end
					token = "update `CSA_Title` SET "..string.format( "Data%02d=%s,Time%02d=%d",i,itemdata[1],i,settime) .. " where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex;
					sasql.query(token);
					char.TalkToCli(charaindex, -1, "该称号已经存在，为您增加了使用时间！", 4);
					char.DelItem(charaindex, haveitemindex);
					return ;
				end
				char.DelItem(charaindex, haveitemindex);
				return ;
			end
		end
		-------------------- 判断称号是否存在结束 --------------------
		-------------------- 称号不存在时，找空位写入 --------------------
		for i=1,50 do--统计到期称号
			local Titleid = other.atoi(sasql.data(3+i));
			if Titleid <= 0 then
				token = "update `CSA_Title` SET "..string.format( "Data%02d=%s,Time%02d=%d",i,itemdata[1],i,settime) .. " where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex;
				sasql.query(token);
				char.TalkToCli(charaindex, -1, "恭喜您，获得["..itemdata[2].."]，可在称号界面点击“显示称号”来更新人物称号！", 4);
				char.DelItem(charaindex, haveitemindex);
				return ;
			end
		end
		
	else
		char.TalkToCli(charaindex, -1, "数据获取失败，请重新尝试，如多次无法获取数据，请联系管理员查看！", 6);
		return;
	end
	
end

function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function reload(charaindex,_data)
	mydata();
	char.setChar(npcindex, "名字",npcdata[1]);
	char.setInt(npcindex, "图像号", npcdata[2]);
	char.setInt(npcindex, "原图像号", npcdata[2]);
	char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
	char.setInt(npcindex, "方向", npcdata[6]);
	char.ToAroundChar(npcindex);
	char.TalkToCli(charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成！", 4);
end

function mydata()
	
	npcdata = {"称号系统",100000,777,37,37,6};
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction("重读称号系统", "reload", "", 1, "[重读称号系统]");
	item.addLUAListFunction( "ITEM_SETTitle", "SetTitle", "");
end

