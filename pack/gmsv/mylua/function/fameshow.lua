function FreeFameShow(meindex)
	local msg = "";
	local token = "";
	--1声望，2气势，3活力，4金币，5积分,6乱舞积分,7体力
	local fmdata = {0,0,0,0,0,0,0,0};
	--获取乱舞积分
	local cdkey = char.getChar(meindex,"帐号");--取得玩家帐号
	local Saveindex = char.getInt(meindex,"存档");--取得玩家序号
	local MyName = char.getChar(meindex,"名字");--取得玩家序号
	local sqlstr = "select Point from `AutoPk` where `Account` = '"..cdkey.."' and `SaveIndex`="..Saveindex;
	local ret = sasql.query(sqlstr);
	if ret == 1 then
		sasql.free_result();
		sasql.store_result();
		local num = sasql.num_rows();
		if num <= 0 then
			--char.TalkToCli(meindex, -1, "您没参加过乱舞活动，正在为您初始化数据...", 6);
			sqlstr = "INSERT INTO `AutoPk` (`Account` ,`SaveIndex` ,`Name`) VALUES ('" .. cdkey.. "',"..Saveindex.. ",'"..MyName.."')";
			sasql.query(sqlstr);
			--char.TalkToCli(meindex, -1, "初始化完成...", 6);
			fmdata[6] = 0;
		else
			sasql.fetch_row();
			fmdata[6] = other.atoi(sasql.data(1));
		end
	end
	fmdata[1] = math.floor(char.getInt(meindex, "声望")/100);--1声望
	fmdata[2] = math.floor(char.getInt(meindex, "气势")/100);--2气势
	fmdata[3] = char.getInt(meindex, "活力");--3活力
	fmdata[4] = sasql.getVipPoint(meindex);--4金币
	fmdata[5] = char.getInt(meindex, "积分");--5积分
	fmdata[6] = char.getInt(meindex, "离线时间");--6
	fmdata[7] = char.getInt(meindex, "体能值");--7
	fmdata[8] = char.getInt(meindex, "刷楼积分");--8
	
	other.CallFunction("FreeGetModeExp", "mylua/function/modeexp.lua", {meindex});
	
	msg = "个人信息详情：";    
	char.TalkToCli(meindex, -1, msg, 1);
	msg = string.format( "个人声望：%-10d个人气势：%-10d个人活力：%-10d", fmdata[1], fmdata[2],fmdata[3]);
	char.TalkToCli(meindex, -1, msg, 4);
	msg = string.format( "金币点数：%-10d积分点数：%-10d离线时间：%-10d", fmdata[4], fmdata[5],fmdata[6]);
	char.TalkToCli(meindex, -1, msg, 4);
	local myfloor = char.getInt(meindex,"地图号");
	if myfloor == 2015 then
		msg = "饲养丛林到期时间："..os.date("%Y-%m-%d %X",char.getWorkInt( meindex, "丛林时间"));
		char.TalkToCli(meindex, -1, msg, 2);
	end
	--msg = string.format( "体力点数：%-10d刷楼积分：%-10d", fmdata[7],fmdata[8]);
	--char.TalkToCli(meindex, -1, msg, 4);
	
	msg = "经验加成详情：";
	char.TalkToCli(meindex, -1, msg, 1);
	lefttime = char.getWorkInt(meindex, "经验时间");--获取经验时间
	msg = string.format( "智慧果加成：%-10s智慧果时效：%-10s", char.getWorkInt( meindex, "经验加成").."%", (lefttime / 60).."分钟");
	char.TalkToCli(meindex, -1, msg, 4);
	--[[
	--天数经验卡
	if char.getInt(meindex,"天数经人时") > os.time() then
		msg = string.format( "人物经验卡：%-10s到期时间：%-10s", char.getInt(meindex,"天数经人倍").."倍", os.date("%Y-%m-%d %X",char.getInt(meindex,"天数经人时")));
	else
		msg = string.format( "人物经验卡：%-10s到期时间：%-10s", "0倍", "未激活");
	end
	char.TalkToCli(meindex, -1, msg, 4);
	if char.getInt(meindex,"天数经宠时") > os.time() then
		msg = string.format( "战宠经验卡：%-10s到期时间：%-10s", char.getInt(meindex,"天数经宠倍").."倍", os.date("%Y-%m-%d %X",char.getInt(meindex,"天数经宠时")));
	else
		msg = string.format( "战宠经验卡：%-10s到期时间：%-10s", "0倍", "未激活");
	end
	char.TalkToCli(meindex, -1, msg, 4);
	if char.getInt(meindex,"天数经骑时") > os.time() then
		msg = string.format( "骑宠经验卡：%-10s到期时间：%-10s", char.getInt(meindex,"天数经骑倍").."倍", os.date("%Y-%m-%d %X",char.getInt(meindex,"天数经骑时")));
	else
		msg = string.format( "骑宠经验卡：%-10s到期时间：%-10s", "0倍", "未激活");
	end
	char.TalkToCli(meindex, -1, msg, 4);
	--]]
	return 1;
end



function data()
	
end

function main()
	data()
end
