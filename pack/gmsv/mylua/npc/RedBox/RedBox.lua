function StringMsg( _Data)--判断是否为数字和字母
	if( string.find(_Data,"'") or string.find(_Data,"\"") or string.find(_Data,"%c") or string.find(_Data,"%p") or string.find(_Data,"%s")) then
		return false;
	else
		return true;
	end
end
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
------------------------------- 外部接入函数 -------------------------------
--_Page红包页码
function FreeRedBoxShow(_charaindex,_Page)--进入查询
	local MyCdKey = char.getChar(_charaindex,"帐号");--取得玩家帐号
	local MyPage = (_Page-1)*8;
	local SqlStr = "select * from `CSA_RedBox` limit "..MyPage..",1000";--查询数据
	local SqlRet = sasql.query(SqlStr);
	local ReBoxList = {};
	local SqlNum = 0;
	if SqlRet == 1 then
		sasql.free_result();
		sasql.store_result();
		SqlNum = sasql.num_rows();
		if SqlNum > 0 then--如果查询信息存在的话，获取信息
			for i=1,8 do
				if i <= SqlNum then
					sasql.fetch_row();
					local RedBoxData = {"","",0,0,0,0,0,0,"",0,""};
					RedBoxData[1] = sasql.data(1);--取得帐号
					RedBoxData[2] = sasql.data(2);--取得名字
					RedBoxData[3] = other.atoi(sasql.data(3));--取得红包金额
					RedBoxData[4] = other.atoi(sasql.data(4));--取得已抢金额
					RedBoxData[5] = other.atoi(sasql.data(5));--取得红包份额
					RedBoxData[6] = other.atoi(sasql.data(6));--取得已抢份额
					RedBoxData[7] = other.atoi(sasql.data(7));--取得对像模式
					RedBoxData[8] = other.atoi(sasql.data(8));--取得领取方式
					RedBoxData[9] = sasql.data(10);--取得时间标识
					RedBoxData[11] = sasql.data(12);--取得家族名
					local TmData = sasql.data(11);--取得已领帐号
					local TmTable = StringSplit(TmData,"|");--分割数据
					for i=1,table.getn(TmTable) do
						if MyCdKey == TmTable[i] then
							RedBoxData[10] = 1;
							break ;
						end
					end
					table.insert(ReBoxList,RedBoxData);
				end
			end
		end
	end
	local MaxPage = math.ceil(SqlNum / 8);
	local token = _Page.."|"..MaxPage.."\n";
	for i=1,table.getn(ReBoxList) do
		token = token .. ReBoxList[i][1] .. "|"..ReBoxList[i][2].."|"..ReBoxList[i][3].."|"..ReBoxList[i][4].."|"..ReBoxList[i][5].."|"..ReBoxList[i][6].."|"..ReBoxList[i][7].."|"..ReBoxList[i][8].."|"..ReBoxList[i][9].."|"..ReBoxList[i][10].."|"..ReBoxList[i][11].."\n";
	end
	print(token);
	lssproto.windows(_charaindex, 1020, 8, 0, char.getWorkInt( npcindex, "对象"), token);
end

------------------------------- NPC窗口函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if _select == 8 then
		return;
	end
	if _seqno == 0 then
		local tmdata = StringSplit(_data,"|");--分割数据
		if tmdata[1] == "要发红包" then
			lssproto.windows(_talkerindex, 1021, 8, 1, char.getWorkInt( _meindex, "对象"), sasql.getVipPoint(_talkerindex));
			return ;
		elseif tmdata[1] == "选择翻页" then
			if table.getn(tmdata) < 2 then
				char.TalkToCli(_talkerindex, -1, "参数有误？？？", 6);
				return ;
			end
			local TmPage = other.atoi(tmdata[2]);
			FreeRedBoxShow(_talkerindex,TmPage)--进入查询
		elseif tmdata[1] == "抢红包" then
			if table.getn(tmdata) < 2 then
				char.TalkToCli(_talkerindex, -1, "参数有误？？？", 6);
				return ;
			end
			local TmTime = tmdata[2];
			local SqlStr = "select * from `CSA_RedBox` where Time='".. TmTime .."';";--查询数据
			local SqlRet = sasql.query(SqlStr);
			if SqlRet == 1 then
				sasql.free_result();
				sasql.store_result();
				local SqlNum = sasql.num_rows();
				if SqlNum > 0 then
					sasql.fetch_row();
					local TmFailyId = char.getInt(_talkerindex,"家族索引");
					local SqlCdkey = sasql.data(1);--取得帐号
					local SqlMaxPoint = other.atoi(sasql.data(3));--取得红包金额
					local SqlPoint = other.atoi(sasql.data(4));--取得已抢金额
					local SqlMaxShare = other.atoi(sasql.data(5));--取得红包份额
					local SqlShare = other.atoi(sasql.data(6));--取得已抢份额
					local SqlMode = other.atoi(sasql.data(7));--取得对像模式
					local SqlType = other.atoi(sasql.data(8));--取得领取方式
					local SqlFailyId = other.atoi(sasql.data(9));--取得家族索引
					local SqlTime = sasql.data(10);--取得红包时间
					local SqlData = sasql.data(11);--取得已领帐号
					local SqlFailyName = sasql.data(12);--取得领取家族
					if SqlPoint >= SqlMaxPoint then
						char.TalkToCli(_talkerindex, -1, "该红包已经被领完了!", 6);
						return ;
					end
					if SqlShare >= SqlMaxShare then
						char.TalkToCli(_talkerindex, -1, "该红包已经被领完了!", 6);
						return ;
					end
					if SqlMode == 0 then
						if TmFailyId ~= SqlFailyId then
							char.TalkToCli(_talkerindex, -1, "你不是 "..SqlFailyName.." 家族正式成员，无法领取!", 6);
							return ;
						end
						if char.getInt(_talkerindex,"家族地位") < 0 then
							char.TalkToCli(_talkerindex, -1, "你还有加入家族，无法领取!", 6);
							return ;
						end
						if char.getInt(_talkerindex,"家族地位") == 2 then
							char.TalkToCli(_talkerindex, -1, "你是 "..SqlFailyName.." 未审核成员，无法领取!", 6);
							return ;
						end
					end
					
					local MyCdKey = char.getChar(_talkerindex,"帐号");--取得玩家帐号
					local TmTable = StringSplit(SqlData,"|");--分割数据
					for i=1,table.getn(TmTable) do
						if MyCdKey == TmTable[i] then
							char.TalkToCli(_talkerindex, -1, "你已经领取过该红包了!", 6);
							return ;
						end
					end
					--将帐号加入已领
					--print(table.getn(TmTable))
					for i=1,table.getn(TmTable) do
						if TmTable[i] == nil or TmTable[i] == "" then
							TmTable[i] = MyCdKey;
							print(TmTable[i])
							break ;
						end
					end
					--计算红包获得
					local GivePoint = 0;
					if SqlType == 0 then
						local TmPoint1 = SqlMaxPoint - SqlPoint;--取得红包剩余金额
						local TmShare1 = SqlMaxShare - SqlShare;--取得红包剩余份数
						local TmPoint2 = math.floor(TmPoint1/TmShare1);--计算出红包平均数
						local TmPoint3 = 0 - TmPoint2;--波动min
						local TmPoint4 = 0 + TmPoint2;--波动max
						local TmPoint5 = math.random(TmPoint3,TmPoint4);
						print(TmPoint3,TmPoint4,TmPoint5);
						GivePoint = TmPoint2 + TmPoint5;--最终结果数
						if GivePoint > TmPoint1 then
							GivePoint = TmPoint1;
						end
						SqlShare = SqlShare + 1;
						if GivePoint <= 0 then
							GivePoint = 1;
						end
						if SqlShare >= SqlMaxShare then
							GivePoint = TmPoint1;
						else
							if GivePoint >= TmPoint1 then
								GivePoint = TmPoint1-1;
							end
						end
						SqlPoint = SqlPoint + GivePoint;
					else
						local TmPoint1 = SqlMaxPoint - SqlPoint;--取得红包剩余金额
						GivePoint = math.floor(SqlMaxPoint/SqlMaxShare);--计算出红包平均数
						SqlShare = SqlShare + 1;
						if SqlShare >= SqlMaxShare then
							GivePoint = TmPoint1;
						end
						SqlPoint = SqlPoint + GivePoint;
					end
					if SqlShare >= SqlMaxShare then--如果是最后一份时，删除数据库信息
						local SqlStr2 = "delete from CSA_RedBox where Time='"..SqlTime.."'";--查询10条数据
						local SqlRet2 = sasql.query(SqlStr2);
						if SqlRet2 ~= 1 then
							char.TalkToCli(_talkerindex, -1, "系统繁忙，请重试[2]！", 6);
							return ;
						end
					else
						local accdata = "";
						for i=1,table.getn(TmTable) do
							if i >= table.getn(TmTable) then
								accdata = accdata .. TmTable[i];
							else
								accdata = accdata .. TmTable[i] .. "|";
							end
						end
						local SqlStr2 = "update `CSA_RedBox` SET Point="..SqlPoint..",Share="..SqlShare..",Data='"..accdata.."' where `Time`= '"..SqlTime.."'";
						local SqlRet2 = sasql.query(SqlStr2);
						if SqlRet2 ~= 1 then
							char.TalkToCli(_talkerindex, -1, "系统繁忙，请重试[2]！", 6);
							return ;
						end
					end
					local MyPoint = sasql.getVipPoint(_talkerindex);
					sasql.setVipPoint(_talkerindex,MyPoint+GivePoint);
					char.TalkToCli(_talkerindex, -1, "抢得红包 ".. GivePoint .." 金币!", 4);
					local logstr = os.date("%Y-%m-%d %X",os.time())
						.."|"..char.getChar(_talkerindex, "账号")
						.."|"..char.getChar(_talkerindex, "名字")
						.."|抢红包"
						.."|红包参数："..SqlPoint.."/"..SqlMaxPoint.."◆"..SqlShare.."/"..SqlMaxShare.."◆"..SqlMode.."◆"..SqlType.."◆"..SqlTime
						.."|抢得："..GivePoint
						.."|金币："..MyPoint.. ">>"..(MyPoint+GivePoint)
						.."\n";
					local f = assert(io.open("./mylua/log/RedBox/"..os.date("%Y%m%d",os.time()).."recv.log", "a"));
					f:write( logstr);
					f:close();
				else
					char.TalkToCli(_talkerindex, -1, "该红包已经不存在了!", 6);
					return ;
				end
			else
				char.TalkToCli(_talkerindex, -1, "系统繁忙，请重试[1]!", 6);
				return ;
			end
			
		end
	elseif _seqno == 1 then
		--1对像模式,2领取方式,3红包个数,4红包金额
		local tmdata = StringSplit(_data,"|");--分割数据
		if table.getn(tmdata) < 4 then
			char.TalkToCli(_talkerindex, -1, "参数有误？？？", 6);
			return ;
		end
		local TmMode = other.atoi(tmdata[1]);
		local TmType = other.atoi(tmdata[2]);
		local TmShare = other.atoi(tmdata[3]);
		local TmPoint = other.atoi(tmdata[4]);
		local MyPoint = sasql.getVipPoint(_talkerindex);
		if TmMode < 0 or TmMode > 1 then
			char.TalkToCli(_talkerindex, -1, "对像模式超出范围？？", 6);
			return ;
		end
		if TmType < 0 or TmType > 1 then
			char.TalkToCli(_talkerindex, -1, "领取方式超出范围？？", 6);
			return ;
		end
		if TmShare < 0 or TmShare > 20 then
			char.TalkToCli(_talkerindex, -1, "红包个数只能在1~20的范围内！", 6);
			return ;
		end
		
		if TmPoint < TmShare then
			char.TalkToCli(_talkerindex, -1, "红包金额不能为0且大于或等于红包个数！", 6);
			return ;
		end
		if TmPoint > 999999 then
			char.TalkToCli(_talkerindex, -1, "红包金额不能大于999999！", 6);
			return ;
		end
		if TmPoint > MyPoint then
			char.TalkToCli(_talkerindex, -1, "你的金币数不足发出 "..TmPoint.." 的红包！", 6);
			return ;
		end
		local TmTime = os.time();
		local TmFailyId = char.getInt(_talkerindex,"家族索引");
		if TmMode == 0 then
			if TmFailyId == -1 then
				char.TalkToCli(_talkerindex, -1, "你没有家族，无法选择家族模式！", 6);
				return ;
			end
			if char.getInt(_talkerindex,"家族地位") == 2 then
				char.TalkToCli(_talkerindex, -1, "你不是 "..SqlFailyName.." 家族正式成员，无法选择家族模式！", 6);
				return ;
			end
		end
		local MyCdKey = char.getChar(_talkerindex,"帐号");--取得玩家帐号
		local MyName = char.getChar(_talkerindex,"名字");--取得玩家帐号
		local MyFailyName = char.getChar(_talkerindex,"家族");--取得玩家帐号
		local SqlStr = "INSERT INTO `CSA_RedBox` (`Account`,`Name`,`MaxPoint`,`Point`,`MaxShare`,`Share`,`Mode`,`Type`,`FaliyId`,`Time`,`Data`,`FaliyName`"
					.. ") VALUES ("
					.. "'" .. MyCdKey .. "',"
					.. "'" .. MyName .. "',"
					.. TmPoint .. ","
					.. "0,"
					.. TmShare .. ","
					.. "0,"
					.. TmMode .. ","
					.. TmType .. ","
					.. TmFailyId .. ","
					.. "'" .. TmTime .. "',"
					.. "'|||||||||||||||||||',"
					.. "'".. MyFailyName .."'"
					..")";
		local SqlRet = sasql.query(SqlStr);
		if SqlRet ~= 1 then
			char.TalkToCli(_talkerindex, -1, "系统繁忙，请重试[1]!", 6);
			return ;
		end
		if TmMode == 0 then
			char.talkToAllServer("[发红包]玩家[".. char.getChar(_talkerindex,"名字").."]发出了一个 "..TmPoint.." 金币的家族红包！", 5);
		else
			char.talkToAllServer("[发红包]玩家[".. char.getChar(_talkerindex,"名字").."]发出了一个 "..TmPoint.." 金币的全服红包！", 5);
		end
		sasql.setVipPoint(_talkerindex,MyPoint-TmPoint);
		char.TalkToCli(_talkerindex, -1, "扣除 ".. TmPoint .." 金币！", "红色");
		local logstr = os.date("%Y-%m-%d %X",os.time())
						.."|"..char.getChar(_talkerindex, "账号")
						.."|"..char.getChar(_talkerindex, "名字")
						.."|发红包"
						.."|红包参数：".."0/"..TmPoint.."◆".."0/"..TmShare.."◆"..TmMode.."◆"..TmType.."◆"..TmTime
						.."|发出："..TmPoint
						.."|金币："..MyPoint.. ">>"..(MyPoint-TmPoint)
						.."\n";
		local f = assert(io.open("./mylua/log/RedBox/"..os.date("%Y%m%d",os.time()).."send.log", "a"));
		f:write( logstr);
		f:close();
	end
end

------------------------------- NPC对话函数 -------------------------------
function Talked(meindex, talkerindex , szMes, color )
	if npc.isFaceToFace(meindex, talkerindex) == 1 then
		FreeRedBox(_talkerindex);
	end
end

------------------------------- 外部CALL函数 -------------------------------
function FreeRedBox(charaindex)
	
	FreeRedBoxShow(charaindex,1);
end

------------------------------- NPC创建函数 -------------------------------
function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function mydata()
	RedStart = true;--红包系统是否开启
	npcdata = {"红包系统",100000,777,37,45,6,"RedBox"};
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

function main()
	mydata()
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 1, "["..npcdata[7].."]");
end


