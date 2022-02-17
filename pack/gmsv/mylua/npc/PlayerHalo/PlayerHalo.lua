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

function FreeHaloShow(charaindex,_num)--进入查询
	--发送当前选择的特效信息
	local restr = _num.."\n";--返回文本
	local maxnum = _num * 10 - 10;
	--1特效编号|2特效名字|3特效形象|4特效血|5特效攻|6特效防|7特效敏|8特效时间
	local cdkey = char.getChar(charaindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(charaindex,"存档");--取得玩家序号
	for i=1,10 do
		local myHalosql = string.format( "Data%02d",maxnum+i)..","..string.format( "Time%02d",maxnum+i);
		local token = "select ".. myHalosql .." from `CSA_Halo` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
		local ret = sasql.query(token);
		if ret == 1 then
			sasql.free_result();
			sasql.store_result();
			local num = sasql.num_rows();
			if num > 0 then--如果查询信息存在的话，获取信息
				sasql.fetch_row();
				local myHaloid = other.atoi(sasql.data(1));--取得特效编号
				local myHalotime = other.atoi(sasql.data(2));--取得特效时间
				if myHaloid >= 0 then
					token = "SELECT * FROM Data_Halo WHERE ID="..myHaloid..";";--从特效编号获取特效数据
					local ret2 = sasql.query(token);
					if ret2 == 1 then
						sasql.free_result();
						sasql.store_result();
						local num2 = sasql.num_rows();
						if num2 > 0 then
							sasql.fetch_row();
							local Haloname = sasql.data(2);--取得特效名字
							local Halohp = sasql.data(3);--取得特效血加成
							local Halostr = sasql.data(4);--取得特效攻加成
							local Halotgh = sasql.data(5);--取得特效防加成
							local Halodex = sasql.data(6);--取得特效敏加成
							local Halomo = sasql.data(7);--取得特效图号
							--1特效编号|2特效名字|3特效图像|4特效加血|5特效加攻|6特效加防|7特效加敏|8特效时间
							restr = restr .. (maxnum+i) .."|"..Haloname.."|"..Halomo.."|"..Halohp.."|"..Halostr.."|"..Halotgh.."|"..Halodex.."|"..myHalotime.."\n";
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
	lssproto.windows(charaindex, 1012, 12, 0, char.getWorkInt( npcindex, "对象"), restr);
end

function InitPlayerHalo(charaindex)--检查玩家特效状态
	local cdkey = char.getChar(charaindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(charaindex,"存档");--取得玩家序号
	local token = "select * from `CSA_Halo` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
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
			--还没有创建特效数据，进入创建
			char.TalkToCli(charaindex, -1, "首次使用特效系统，正在为您初始化特效数据...", 6);
			token = "INSERT INTO `CSA_Halo` (`Account` ,`SaveIndex`) VALUES ('" .. cdkey.. "',"..Saveindex.. ")";
			sasql.query(token);
			char.TalkToCli(charaindex, -1, "初始化完成，请重新使用特效功能！", 6);
			return;
		end
		sasql.fetch_row();
		local mytime = os.time();
		local delHalo = {};
		for i=1,50 do--统计到期特效
			local Haloid = other.atoi(sasql.data(3+i));
			local Halotime = other.atoi(sasql.data(53+i));
			if Halotime > 0 then
				if Halotime < mytime then
					table.insert(delHalo,i);
					--移除特效效果
					char.setInt(charaindex,"现光环属性",0);
					char.setInt(charaindex,"现光环图像",0);
					char.complianceParameter(charaindex);
					char.ToAroundChar(charaindex);
					char.Updata(charaindex,29158);
					--char.sendArroundCharaData(charaindex);--更新特效
					char.WarpToSpecificPoint(charaindex, char.getInt(charaindex, "地图号"), char.getInt(charaindex, "坐标X"), char.getInt(charaindex, "坐标Y"));
					char.TalkToCli(charaindex, -1, "您第"..i.."号特效已到期，系统自动清除！", 6);
				end
			end
		end
		--开始删除
		token = "update `CSA_Halo` SET ";
		for i=1,table.getn(delHalo) do
			if i >= table.getn(delHalo) then
				token = token .. string.format( "Data%02d=0,Time%02d=0",delHalo[i],delHalo[i]);
			else
				token = token .. string.format( "Data%02d=0,Time%02d=0,",delHalo[i],delHalo[i]);
			end
		end
		token = token .. " where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex;
		sasql.query(token);
	else
		char.TalkToCli(charaindex, -1, "数据获取失败，请重新尝试，如多次无法获取数据，请联系管理员查看！", 6);
		return;
	end
	
	
end

function FreeHalo(charaindex)
	InitPlayerHalo(charaindex);
	FreeHaloShow(charaindex,1);
end

function WindowTalked ( _meindex, _talkerindex, _seqno, _select, _data)
	if _seqno == 0 then
		if (_select==16) then--上一页
			local Halopage = other.atoi(_data)-1;
			if Halopage < 1 then
				Halopage = 5;
			end
			FreeHaloShow(_talkerindex,Halopage);
		elseif (_select==32) then--上一页
			local Halopage = other.atoi(_data)+1;
			if Halopage > 5 then
				Halopage = 1;
			end
			FreeHaloShow(_talkerindex,Halopage);
		else
			local Halodata = StringSplit(_data, "|");--分割客户端传递信息
			if table.getn(Halodata) < 2 then
				return;
			end
			if Halodata[1] == "显示特效" then
				local Haloid = other.atoi(Halodata[2]);
				local cdkey = char.getChar(_talkerindex,"帐号");--取得玩家帐号
				local Saveindex = char.getInt(_talkerindex,"存档");--取得玩家序号
				local myHalosql = string.format( "Data%02d",Haloid)..","..string.format( "Time%02d",Haloid);
				local token = "select "..myHalosql.." from `CSA_Halo` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
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
						local myHaloid = other.atoi(sasql.data(1));--取得特效编号
						local myHalotime = other.atoi(sasql.data(2));--取得特效时间
						if myHalotime > 0 then
							if myHalotime < os.time() then--特效到期，删除特效
								char.TalkToCli(_talkerindex, -1, "该特效已经到期，无法使用!", 6);
								return ;
							end
						end
						if myHaloid <= 0 then
							char.TalkToCli(_talkerindex, -1, "？？？", 6);
							return ;
						end
						--取得特效加成数据
						local token2 = "SELECT * FROM Data_Halo WHERE ID="..myHaloid..";";--从特效编号获取特效数据
						local ret2 = sasql.query(token2);
						if ret2 == 1 then
							sasql.free_result();
							sasql.store_result();
							local num2 = sasql.num_rows();
							if num2 <= 0 then
								char.TalkToCli(_talkerindex, -1, "该特效数据未存在！", 6);
								return;
							end
							sasql.fetch_row();
							local Haloname = sasql.data(2);--取得特效名字
							local Halohp = sasql.data(3);--取得特效血加成
							local Halostr = sasql.data(4);--取得特效攻加成
							local Halotgh = sasql.data(5);--取得特效防加成
							local Halodex = sasql.data(6);--取得特效敏加成
							local Halomo = sasql.data(7);--取得特效图号
													
							local Halojc = char.getLiftTo8(Halohp, 1) + char.getLiftTo8(Halostr, 2) + char.getLiftTo8(Halotgh, 3) + char.getLiftTo8(Halodex, 4);
							char.setInt(_talkerindex,"现光环属性", Halojc);
							char.setInt(_talkerindex,"现光环图像",Halomo);
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
			elseif Halodata[1] == "取消显示" then
				--移除特效效果
				local Halojc = char.getLiftTo8(0, 1) + char.getLiftTo8(0, 2) + char.getLiftTo8(0, 3) + char.getLiftTo8(0, 4);
				char.setInt(_talkerindex,"现光环属性",0);
				char.setInt(_talkerindex,"现光环图像",0);
				char.complianceParameter(_talkerindex);
				char.ToAroundChar(_talkerindex);
				char.Updata(_talkerindex,29158);
				--char.sendArroundCharaData(_talkerindex);--更新特效
				char.WarpToSpecificPoint(_talkerindex, char.getInt(_talkerindex, "地图号"), char.getInt(_talkerindex, "坐标X"), char.getInt(_talkerindex, "坐标Y"));
				char.TalkToCli(_talkerindex, -1, "已取消特效显示！", 4);
			end
		end
	end
end

function SetHalo(itemindex, charaindex, toindex, haveitemindex)
	local itemdata = StringSplit(item.getChar(itemindex, "字段"),"|");
	--1特效编号,2特效名字,3特效时间
	if table.getn(itemdata) < 3 then
		char.TalkToCli(_talkerindex, -1, "该道具有问题，无法使用！", 4);
		return ;
	end
	local cdkey = char.getChar(charaindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(charaindex,"存档");--取得玩家序号
	local token = "select * from `CSA_Halo` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
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
			--还没有创建特效数据，进入创建
			char.TalkToCli(charaindex, -1, "首次使用特效系统，正在为您初始化特效数据...", 6);
			token = "INSERT INTO `CSA_Halo` (`Account` ,`SaveIndex`) VALUES ('" .. cdkey.. "',"..Saveindex.. ")";
			sasql.query(token);
			char.TalkToCli(charaindex, -1, "初始化完成，请重新使用特效道具！", 6);
			return;
		end
		sasql.fetch_row();
		local mytime = os.time();
		local delHalo = {};
		local settime = 0;
		if other.atoi(itemdata[3]) > 0 then
			settime = os.time()+(other.atoi(itemdata[3])*60*60*24);
		else
			settime = 0;
		end
		-------------------- 判断特效是否存在 --------------------
		for i=1,50 do--统计到期特效
			local Haloid = other.atoi(sasql.data(3+i));
			local HaloTime = other.atoi(sasql.data(53+i));
			if Haloid == other.atoi(itemdata[1]) then
				if HaloTime > 0 then
					if other.atoi(itemdata[3]) > 0 then
						settime = HaloTime+(other.atoi(itemdata[3])*60*60*24);
						if settime < mytime then
							settime = os.time()+(other.atoi(itemdata[3])*60*60*24);
						end
					else
						settime = 0;
					end
					token = "update `CSA_Halo` SET "..string.format( "Data%02d=%s,Time%02d=%d",i,itemdata[1],i,settime) .. " where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex;
					sasql.query(token);
					char.TalkToCli(charaindex, -1, "该特效已经存在，为您增加了使用时间！", 4);
					char.DelItem(charaindex, haveitemindex);
					return ;
				end
				char.DelItem(charaindex, haveitemindex);
				return ;
			end
		end
		-------------------- 判断皮肤是否存在结束 --------------------
		for i=1,50 do--统计到期特效
			local Haloid = other.atoi(sasql.data(3+i));
			if Haloid <= 0 then
				token = "update `CSA_Halo` SET "..string.format( "Data%02d=%s,Time%02d=%d",i,itemdata[1],i,settime) .. " where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex;
				sasql.query(token);
				char.TalkToCli(charaindex, -1, "恭喜您，获得["..itemdata[2].."]，可在特效界面点击“显示特效”来更新人物特效！", 4);
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
	npcdata = {"特效系统",100000,777,37,39,6};
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction("重读特效系统", "reload", "", 1, "[重读特效系统]");
	item.addLUAListFunction( "ITEM_SETHalo", "SetHalo", "");
end

