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

function getNOindex( baseNo)--通过形象获取形象索引
	if baseNo >= 100000 and baseNo < 100240 then
		metamo = baseNo - 100000;
		for i = 1, 12 do
			if metamo >=  (i-2) * 20 and metamo <  i * 20 then
				return i;
			end
		end
	elseif baseNo >= 100700 and baseNo < 100820 then
		metamo = baseNo - 100700;
		for i = 1, 12 do
			if metamo >=  (i-2) * 10 and metamo <  i * 10 then
				return i;
			end
		end
	elseif baseNo >= 100400 and baseNo < 100410 then
		return 13;
	end
	return -1;
end

function FreeSkinShow(charaindex,_num)--进入查询
	--发送当前选择的皮肤信息
	local restr = _num.."\n";--返回文本
	local maxnum = _num * 10 - 10;
	--1皮肤编号|2皮肤名字|3皮肤形象|4皮肤血|5皮肤攻|6皮肤防|7皮肤敏|8皮肤时间
	local cdkey = char.getChar(charaindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(charaindex,"存档");--取得玩家序号
	for i=1,10 do
		local myskinsql = string.format( "Data%02d",maxnum+i)..","..string.format( "Time%02d",maxnum+i);
		local token = "select ".. myskinsql .." from `CSA_Skin` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
		local ret = sasql.query(token);
		if ret == 1 then
			sasql.free_result();
			sasql.store_result();
			local num = sasql.num_rows();
			if num > 0 then--如果查询信息存在的话，获取信息
				sasql.fetch_row();
				local myskinid = other.atoi(sasql.data(1));--取得皮肤编号
				local myskintime = other.atoi(sasql.data(2));--取得皮肤时间
				if myskinid >= 0 then
					token = "SELECT * FROM Data_Skin WHERE ID="..myskinid..";";--从皮肤编号获取皮肤数据
					local ret2 = sasql.query(token);
					if ret2 == 1 then
						sasql.free_result();
						sasql.store_result();
						local num2 = sasql.num_rows();
						if num2 > 0 then
							sasql.fetch_row();
							local skinname = sasql.data(2);--取得皮肤名字
							local skinhp = sasql.data(3);--取得皮肤血加成
							local skinstr = sasql.data(4);--取得皮肤攻加成
							local skintgh = sasql.data(5);--取得皮肤防加成
							local skindex = sasql.data(6);--取得皮肤敏加成
							local skinmo = {};
							for i=7,18 do
								table.insert(skinmo,sasql.data(i));
							end
							local playno = char.getInt(charaindex,"原图像号");--获取玩家形象
							local playerid = getNOindex( playno);--取得角色编号
							--1皮肤编号|2皮肤名字|3皮肤图像|4皮肤加血|5皮肤加攻|6皮肤加防|7皮肤加敏|8皮肤时间
							restr = restr .. (maxnum+i) .."|"..skinname.."|"..skinmo[playerid].."|"..skinhp.."|"..skinstr.."|"..skintgh.."|"..skindex.."|"..myskintime.."\n";
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
	lssproto.windows(charaindex, 1010, 12, 0, char.getWorkInt( npcindex, "对象"), restr);
end

function InitPlayerSkin(charaindex)--检查玩家皮肤状态
	local cdkey = char.getChar(charaindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(charaindex,"存档");--取得玩家序号
	local token = "select * from `CSA_Skin` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
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
			--还没有创建皮肤数据，进入创建
			char.TalkToCli(charaindex, -1, "首次使用皮肤系统，正在为您初始化皮肤数据...", 6);
			token = "INSERT INTO `CSA_Skin` (`Account` ,`SaveIndex`) VALUES ('" .. cdkey.. "',"..Saveindex.. ")";
			sasql.query(token);
			char.TalkToCli(charaindex, -1, "初始化完成，请重新使用皮肤功能！", 6);
			return;
		end
		sasql.fetch_row();
		local mytime = os.time();
		local delskin = {};
		for i=1,50 do--统计到期皮肤
			local skintime = other.atoi(sasql.data(23+i));
			local skinid = other.atoi(sasql.data(3+i));
			if skintime > 0 then
				if skintime < mytime then
					table.insert(delskin,i);
					--移除皮肤效果
					char.setInt(charaindex,"皮肤属性",0);
					char.setInt(charaindex,"图像号",char.getInt(charaindex,"原图像号"));
					char.complianceParameter(charaindex);
					char.ToAroundChar(charaindex);
					char.Updata(charaindex,29158);
					char.WarpToSpecificPoint(charaindex, char.getInt(charaindex, "地图号"), char.getInt(charaindex, "坐标X"), char.getInt(charaindex, "坐标Y"));
					char.TalkToCli(charaindex, -1, "您第"..i.."号皮肤已到期，系统自动清除！", 6);
				end
			end
		end
		--开始删除
		token = "update `CSA_Skin` SET ";
		for i=1,table.getn(delskin) do
			if i >= table.getn(delskin) then
				token = token .. string.format( "Data%02d=0,Time%02d=0",delskin[i],delskin[i]);
			else
				token = token .. string.format( "Data%02d=0,Time%02d=0,",delskin[i],delskin[i]);
			end
		end
		token = token .. " where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex;
		sasql.query(token);
	else
		char.TalkToCli(charaindex, -1, "数据获取失败，请重新尝试，如多次无法获取数据，请联系管理员查看！", 6);
		return;
	end
	
	
end

function FreeSkin(charaindex)
	InitPlayerSkin(charaindex);
	FreeSkinShow(charaindex,1);
end

function WindowTalked ( _meindex, _talkerindex, _seqno, _select, _data)
	if _seqno == 0 then
		if (_select==16) then--上一页
			local skinpage = other.atoi(_data)-1;
			if skinpage < 1 then
				skinpage = 5;
			end
			FreeSkinShow(_talkerindex,skinpage);
		elseif (_select==32) then--上一页
			local skinpage = other.atoi(_data)+1;
			if skinpage > 5 then
				skinpage = 1;
			end
			FreeSkinShow(_talkerindex,skinpage);
		else
			local skindata = StringSplit(_data, "|");--分割客户端传递信息
			if table.getn(skindata) < 2 then
				return;
			end
			if skindata[1] == "显示皮肤" then
				if char.getInt(_talkerindex,"骑宠") < 0 then
					char.TalkToCli(_talkerindex, -1, "只能在骑乘状态下才能显示皮肤！", 6);
					return ;
				end
				local skinid = other.atoi(skindata[2]);
				local cdkey = char.getChar(_talkerindex,"帐号");--取得玩家帐号
				local Saveindex = char.getInt(_talkerindex,"存档");--取得玩家序号
				local myskinsql = string.format( "Data%02d",skinid)..","..string.format( "Time%02d",skinid);
				local token = "select "..myskinsql.." from `CSA_Skin` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
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
						local myskinid = other.atoi(sasql.data(1));--取得皮肤编号
						local myskintime = other.atoi(sasql.data(2));--取得皮肤时间
						if myskintime > 0 then
							if myskintime < os.time() then--皮肤到期，删除皮肤
								char.TalkToCli(_talkerindex, -1, "该皮肤已经到期，无法使用!", 6);
								return ;
							end
						end
						if myskinid <= 0 then
							char.TalkToCli(_talkerindex, -1, "？？？", 6);
							return ;
						end
						--取得皮肤加成数据
						local token2 = "SELECT * FROM Data_Skin WHERE ID="..myskinid..";";--从皮肤编号获取皮肤数据
						local ret2 = sasql.query(token2);
						if ret2 == 1 then
							sasql.free_result();
							sasql.store_result();
							local num2 = sasql.num_rows();
							if num2 <= 0 then
								char.TalkToCli(_talkerindex, -1, "该皮肤数据未存在！", 6);
								return;
							end
							sasql.fetch_row();
							local skinname = sasql.data(2);--取得皮肤名字
							local skinhp = sasql.data(3);--取得皮肤血加成
							local skinstr = sasql.data(4);--取得皮肤攻加成
							local skintgh = sasql.data(5);--取得皮肤防加成
							local skindex = sasql.data(6);--取得皮肤敏加成
							
							local skinmo = {};
							for i=7,18 do
								table.insert(skinmo,sasql.data(i));
							end
							local playno = char.getInt(_talkerindex,"原图像号");--获取玩家形象
							local playerid = getNOindex( playno);--取得角色编号							
							local skinjc = char.getLiftTo8(skinhp, 1) + char.getLiftTo8(skinstr, 2) + char.getLiftTo8(skintgh, 3) + char.getLiftTo8(skindex, 4);
							char.setInt(_talkerindex, "皮肤属性", skinjc);
							char.setInt(_talkerindex,"图像号",skinmo[playerid]);
							char.complianceParameter(_talkerindex);
							char.Updata(_talkerindex,29158);
							char.ToAroundChar(_talkerindex);
							char.WarpToSpecificPoint(_talkerindex, char.getInt(_talkerindex, "地图号"), char.getInt(_talkerindex, "坐标X"), char.getInt(_talkerindex, "坐标Y"));
						end
					end
				else
					char.TalkToCli(_talkerindex, -1, "数据获取失败，请重新尝试，如多次无法获取数据，请联系管理员查看！", 6);
					return;
				end
			elseif skindata[1] == "取消显示" then
				--移除皮肤效果
				--local skinjc = char.getLiftTo8(0, 1) + char.getLiftTo8(0, 2) + char.getLiftTo8(0, 3) + char.getLiftTo8(0, 4);
				char.setInt(_talkerindex,"皮肤属性",0);
				char.setInt(_talkerindex,"骑宠",-1);
				char.Updata(_talkerindex,"骑宠");
				char.setInt(_talkerindex,"图像号",char.getInt(_talkerindex,"原图像号"));
				char.complianceParameter(_talkerindex);
				char.ToAroundChar(_talkerindex);
				char.Updata(_talkerindex,29158);
				char.WarpToSpecificPoint(_talkerindex, char.getInt(_talkerindex, "地图号"), char.getInt(_talkerindex, "坐标X"), char.getInt(_talkerindex, "坐标Y"));
				char.TalkToCli(_talkerindex, -1, "已取消皮肤显示，取消皮肤显示会自动下骑，请自行重新骑上！", 4);
			end
		end
	end
end

function SetSkin(itemindex, charaindex, toindex, haveitemindex)
	local itemdata = StringSplit(item.getChar(itemindex, "字段"),"|");
	--1皮肤编号,2皮肤名字,3皮肤时间
	if table.getn(itemdata) < 3 then
		char.TalkToCli(_talkerindex, -1, "该道具有问题，无法使用！", 4);
		return ;
	end
	local cdkey = char.getChar(charaindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(charaindex,"存档");--取得玩家序号
	local token = "select * from `CSA_Skin` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
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
			--还没有创建皮肤数据，进入创建
			char.TalkToCli(charaindex, -1, "首次使用皮肤系统，正在为您初始化皮肤数据...", 6);
			token = "INSERT INTO `CSA_Skin` (`Account` ,`SaveIndex`) VALUES ('" .. cdkey.. "',"..Saveindex.. ")";
			sasql.query(token);
			char.TalkToCli(charaindex, -1, "初始化完成，请重新使用皮肤道具！", 6);
			return;
		end
		sasql.fetch_row();
		local mytime = os.time();
		local delskin = {};
		local settime = 0;
		if other.atoi(itemdata[3]) > 0 then
			settime = os.time()+(other.atoi(itemdata[3])*60*60*24);
		else
			settime = 0;
		end
		-------------------- 判断皮肤是否存在 --------------------
		for i=1,50 do--统计到期皮肤
			local skinid = other.atoi(sasql.data(3+i));
			local skinTime = other.atoi(sasql.data(53+i));
			if skinid == other.atoi(itemdata[1]) then
				if skinTime > 0 then
					if other.atoi(itemdata[3]) > 0 then
						settime = skinTime+(other.atoi(itemdata[3])*60*60*24);
						if settime < mytime then
							settime = os.time()+(other.atoi(itemdata[3])*60*60*24);
						end
					else
						settime = 0;
					end
					token = "update `CSA_Skin` SET "..string.format( "Data%02d=%s,Time%02d=%d",i,itemdata[1],i,settime) .. " where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex;
					sasql.query(token);
					char.TalkToCli(charaindex, -1, "该皮肤已经存在，为您增加了使用时间！", 4);
					char.DelItem(charaindex, haveitemindex);
					return ;
				end
				char.DelItem(charaindex, haveitemindex);
				return ;
			end
		end
		-------------------- 判断皮肤是否存在结束 --------------------
		for i=1,50 do--统计到期皮肤
			local skinid = other.atoi(sasql.data(3+i));
			if skinid <= 0 then
				token = "update `CSA_Skin` SET "..string.format( "Data%02d=%s,Time%02d=%d",i,itemdata[1],i,settime) .. " where `Account`= '"..cdkey.."' and `SaveIndex`="..Saveindex;
				sasql.query(token);
				char.TalkToCli(charaindex, -1, "恭喜您，获得["..itemdata[2].."]，可在皮肤界面点击“显示皮肤”来更新人物皮肤！", 4);
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
	npcdata = {"皮肤系统",100000,777,37,41,6};
end

function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction("重读皮肤系统", "reload", "", 1, "[重读皮肤系统]");
	item.addLUAListFunction( "ITEM_SETSKIN", "SetSkin", "");
end

