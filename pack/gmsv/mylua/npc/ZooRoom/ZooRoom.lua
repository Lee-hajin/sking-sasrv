------------------------------- 杂类模块 -------------------------------
function StringSplit(_data, _splitint)--分割处理函数（返回数组）
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

--从字段处获取道具是否存在，存在的情况下返回道具位置，不存在返回-1
function FreeItemForUseFunc(_talkerindex,_checkstr)
	for i=9,23 do
		local itemindex = char.getItemIndex(_talkerindex,i);
		if itemindex ~= -1 then
			local tmUseFunc = item.getChar(itemindex, "使用函数名");
			if tmUseFunc == _checkstr then
				return i;
			end
		end
	end
	return -1;
end

------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		return ;
	end
	if _select == 8 then
		return ;
	end
	if (_seqno == 0) then
		if char.getWorkInt(_talkerindex,"组队") > 0 then
			char.TalkToCli(_talkerindex, _meindex, "不可组队传送！", 6);
			return;
		end
		local itemid = FreeItemForUseFunc(_talkerindex,"ITEM_ZooRoomTime");
		if itemid == -1 then
			char.TalkToCli(_talkerindex, -1, "你身上并没有[饲养丛林门票]，无法进入!", 6);
			return;
		end
		local randxy = map.RandXAndY(2015);--返回一个随机坐标数组
		if table.getn(randxy) > 1 then
			local randx = randxy[1];
			local randy = randxy[2];
			local itemindex = char.getItemIndex(_talkerindex,itemid);
			if itemindex <= -1 then
				char.TalkToCli(_talkerindex, -1, "你身上并没有[饲养丛林门票]，无法进入!", 6);
				return;
			end
			local ItemTime = other.atoi(item.getChar(itemindex, "字段"));
			local mytime = char.getWorkInt( _talkerindex, "丛林时间");
			if mytime < os.time() then
				mytime = os.time();
			end
			mytime = mytime + (ItemTime * 60);
			char.setWorkInt( _talkerindex, "丛林时间",mytime);
			
			char.DelItem(_talkerindex, itemid);	
			char.TalkToCli(_talkerindex, -1, "交出 饲养丛林门票，获得饲养丛林时间："..ItemTime.."分钟！", 4);
			char.WarpToSpecificPoint(_talkerindex, 2015, randx, randy);--将玩家传送进去
		end
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	--char.TalkToCli(_talkerindex, -1, "写出外挂地图格式！", 4);
	--map.WalkAbleForJL(_talkerindex);
	--char.TalkToCli(_talkerindex, -1, "写出完成！", 4);
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local token = "                [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n你好！"
					.."\n只要你拥有[style c=5]饲养丛林门票[/style]"
					.."\n我就可以将你传送去饲养丛林里！"
					.."\n饲养丛林里可以在有限时间内捕捉到你需要的宠物！"
					.."\n[style c=6]PS：在丛林中如果登出，再次登录回被传回记录点哦！[/style]";
		lssproto.windows(_talkerindex, 0, 12, 0, char.getWorkInt( _meindex, "对象"), token);
	end
end

------------------------------- 循环事件函数 -------------------------------
function Loop(meindex)--每秒一次
	LoopTime = LoopTime + 1;
	if LoopTime > 10 then--每10秒执行一次(最好不要太频繁)
		local PlayerList = map.getFloorPlayer(2015);--取得丛林内玩家列表
		if table.getn(PlayerList) ~= nil then
			if table.getn(PlayerList) > 0 then
				for i=1,table.getn(PlayerList) do--开始遍历玩家
					local Tmfloor = char.getInt(PlayerList[i],"地图号");--重新取地图号
					if Tmfloor == 2015 then--再次判断地图号，防止遍历时间过长
						local TmTime = char.getWorkInt( PlayerList[i], "丛林时间");--取出时间
						if TmTime <= os.time() then--如果玩家的时间小于当前时间
							local itemid = FreeItemForUseFunc(PlayerList[i],"ITEM_ZooRoomTime");--寻找玩家身上是否还有门票
							if itemid > 0 then--如果有门票，扣除门票，增长时间
								local itemindex = char.getItemIndex(PlayerList[i],itemid);
								if itemindex <= -1 then--索引有误
									char.setWorkInt( PlayerList[i], "丛林时间",0);
									char.DischargeParty(PlayerList[i], 1);--解散组队
									char.WarpToSpecificPoint(PlayerList[i], 2000, 44, 67);--将玩家传送出去
									char.TalkToCli(PlayerList[i], -1, "饲养丛林时间已到，你身上已经没有门票了，现在将你传送出去！", 4);
								else
									local ItemTime = other.atoi(item.getChar(itemindex, "字段"));
									local mytime = char.getWorkInt( PlayerList[i], "丛林时间");
									if mytime < os.time() then
										mytime = os.time();
									end
									mytime = mytime + (ItemTime * 60);
									char.setWorkInt( PlayerList[i], "丛林时间",mytime);
									char.DelItem(PlayerList[i], itemid);
									char.TalkToCli(PlayerList[i], -1, "饲养丛林时间已到，发现你身上还有门票，自动为你使用！", 4);
								end
							else--没有门票了，直接传送出去
								char.setWorkInt( PlayerList[i], "丛林时间",0);
								char.DischargeParty(PlayerList[i], 1);--解散组队
								char.WarpToSpecificPoint(PlayerList[i], 2000, 44, 67);--将玩家传送出去
								char.TalkToCli(PlayerList[i], -1, "饲养丛林时间已到，你身上已经没有门票了，现在将你传送出去！", 4);
							end
						end
					end
				end
			end
		end
		LoopTime = 0;
	end
end

------------------------------- 主NPC创建事件函数 -------------------------------
function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
	char.setFunctionPointer(npcindex, "循环事件", "Loop", "");
end

------------------------------- 道具使用函数 -------------------------------
function ZooRoomTimeCall(itemindex, charaindex, toindex, haveitemindex)
	local Tmfloor = char.getInt(charaindex,"地图号");--重新取地图号
	if Tmfloor == 2015 then--再次判断地图号，防止遍历时间过长
		local ItemTime = other.atoi(item.getChar(itemindex, "字段"));
		local mytime = char.getWorkInt( charaindex, "丛林时间");
		if mytime < os.time() then
			mytime = os.time();
		end
		mytime = mytime + (ItemTime * 60);
		char.setWorkInt( charaindex, "丛林时间",mytime);
		char.DelItem(charaindex, haveitemindex);
		char.TalkToCli(charaindex, -1, "使用饲养丛林门票，时间增加".. ItemTime .."分钟！", 4);
	else
		char.TalkToCli(charaindex, -1, "饲养丛林门票只能在饲养丛林里使用！", 6);
	end
end
------------------------------- GM命令函数 -------------------------------
function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.setChar(npcindex, "名字",npcdata[1]);
		char.setInt(npcindex, "图像号", npcdata[2]);
		char.setInt(npcindex, "原图像号", npcdata[2]);
		char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
		char.setInt(npcindex, "方向", npcdata[6]);
		char.ToAroundChar(npcindex);
		char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成", 6);
	end
end

------------------------------- 常量设置函数 -------------------------------
function mydata()
	npcdata = {"饲养丛林管理",51183,777,42,25,6,"ZooRoom"};
	LoopTime = 0;--计时器用
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
	item.addLUAListFunction( "ITEM_ZooRoomTime", "ZooRoomTimeCall", "");
end
