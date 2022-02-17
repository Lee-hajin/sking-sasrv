function Loop1(meindex)
	LoopTime = LoopTime + 1;
	if math.mod(LoopTime,DelPointTime[1]) == 0 then--创建小怪时间判断
		char.setInt(meindex, "离线时间",char.getInt(meindex, "离线时间")-DelPointTime[2]);
	end
	if char.getWorkInt(meindex, "战斗") == 0 then
		local enemytable = {0, 0, 0, 0, 0, 0, 0, 0, 0 };
		battleindex = battle.CreateVsEnemy(meindex, -1, enemytable);
	end
	if char.getInt(meindex, "离线时间") <= 0 then
		char.logou(meindex);
	end
end

function Loop2(meindex)
	LoopTime = LoopTime + 1;
	if math.mod(LoopTime,DelPointTime[1]) == 0 then--创建小怪时间判断
		char.setInt(meindex, "离线时间",char.getInt(meindex, "离线时间")-DelPointTime[2]);
	end
	
	if char.getInt(meindex, "离线时间") <= 0 then--离线时间不足的时候登出
		char.logou(meindex);
	end

	if char.getWorkInt(meindex, "组队") == 0 then--组队解散的时候登出
		char.logou(meindex);
	end

end

function OffLine(charaindex)
	--if char.getWorkInt(charaindex, "组队") == 0 then
		--char.TalkToCli(charaindex, -1, "离线挂机只能在组队状态下使用！", "红色");
		--return 1;
	--end
	if char.getWorkInt(charaindex, "组队") == 1 then
		if char.getWorkInt(charaindex,"战斗") ~= 0 then
			char.TalkToCli(charaindex, -1, "使用离线挂机的时候请勿用原地遇敌，在非战斗中使用！", "红色");
			return 1;
		end
	end
	if char.getWorkInt(charaindex, "摆摊") == -1 then
		if char.getInt(charaindex, "离线时间") <= 0 then
			char.setInt(charaindex, "离线时间",0);
			char.TalkToCli(charaindex, -1, "您的离线时间小于0，无法进行离线挂机！", "红色");
			return 1;
		end
	end
	local token="";
	if config.checkIp(char.userip(charaindex)) == 1 then
		if char.getWorkChar(charaindex, "MAC") ~= "" then--判断MAC
			token = "SELECT `*` FROM `CSA_Login` ".. "WHERE `MAC1` = '" ..  char.getWorkChar(charaindex, "MAC") .. "' and `Online` > 0 and `Offline` = 1";
		end
	else
		if char.getWorkChar(charaindex, "MAC") ~= "" then--判断MAC
			token = "SELECT `*` FROM `CSA_Login` ".. "WHERE (`MAC1` = '" ..  char.getWorkChar(charaindex, "MAC") .. "' or `IP` = '"..net.userip(charaindex) .."') and `Online` > 0 and `Offline` = 1";
		else--判断IP
			token = "SELECT `*` FROM `CSA_Login` ".. "WHERE `IP` = '"..net.userip(charaindex) .."' and `Online` > 0 and `Offline` = 1";
		end
	end
	if token ~= "" then
		local ret = sasql.query(token);
		if ret == 1 then
			sasql.free_result();
			sasql.store_result();
			local num = sasql.num_rows();
			if num > 0 then
				if num >= MaxOffLineNum then
					char.TalkToCli(charaindex, -1, "你的离线数量有" .. MaxOffLineNum .. "个，请自觉维护游戏平衡！", "黄色");
					return 1;
				end
			end
		end
	end
	if char.getWorkInt(charaindex, "摆摊") == -1 then
		if char.getWorkInt(charaindex, "组队") ~= 2 then--如果不是队员的话，执行遇敌的循环函数
			char.setFunctionPointer(charaindex, "循环事件", "Loop1", "");
			char.setInt(charaindex, "循环事件时间", 1000);
		else--如果是队员的话，执行平台的循环函数
			char.setFunctionPointer(charaindex, "循环事件", "Loop2", "");
			char.setInt(charaindex, "循环事件时间", 1000);
		end
	end
	char.TalkToCli(charaindex, -1, "您现在开启了离线挂机！", "黄色");
	char.setWorkInt(charaindex, "登陆时间", other.time());
	char.setWorkInt(charaindex, "离线", 1);
	sql = "update `CSA_Login` set `Offline`=1 where `Name`='" .. char.getChar(charaindex,"账号") .. "'";
	sasql.query(sql);
	fd = char.getFd(charaindex);
	net.setCloseRequest(fd,1);
	other.setLuaPLayerNum(other.getLuaPLayerNum()+1);
	return 1;
end


function mydata()
	--设置变量
	DelPointTime = {60,1};--每多少秒扣除多少点，60,1表示每60秒扣1点
	MaxOffLineNum = 10;--最大离线数量
	--临时变量
	LoopTime = 0;
end

function main()
	mydata();
	--item.addLUAListFunction( "ITEM_OffLineTime", "OffLineTime", "");
end
