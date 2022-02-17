--经验计算
function FreeGetModeExp( charindex)
	local battleexp = config.getBattleexp();--获取线路经验倍数
	local tmtime = other.atoi(os.time());
	local Tmweekday = other.atoi(os.date("%w",os.time()));--得到星期
	local msg = "";
	--print(config.getShareExp());
	local ExpShare = config.getShareExp();--获取线路是否经验共享
	if ExpShare == 1 then
		msg = "["..config.getGameservername().."]本线经验倍数："..battleexp.."倍[经验共享]";
	else
		msg = "["..config.getGameservername().."]本线经验倍数："..battleexp.."倍";
	end
	local TmBase = {0,0};
	for i=1,table.getn(WeekDay) do
		if WeekDay[i][1] == Tmweekday then
			TmBase[1] = WeekDay[i][1];
			TmBase[2] = WeekDay[i][2];
			break ;
		end
	end
	
	if TmBase[2] > 0 then
		if ExpShare == 1 then
			msg = "[" .. config.getGameservername() .. "]本线经验倍数：" .. battleexp .. "倍[经验共享](周" .. TmBase[1] .. "经验活动".. TmBase[2] .."倍)";
		else
			msg = "[" .. config.getGameservername() .. "]本线经验倍数：" .. battleexp .. "倍(周" .. TmBase[1] .. "经验活动".. TmBase[2] .."倍)";
		end
	end
	if TmBase[2] <= 0 then
		if ExpUpTime[1] > 0 then
			if tmtime >= ExpUpTime[2] and tmtime <= ExpUpTime[3] then
				if ExpShare == 1 then
					msg = "[" .. config.getGameservername() .. "]本线经验倍数：" .. battleexp .. "倍[经验共享]("..ExpUpTime[4] .. "+" .. ExpUpTime[1] .. "倍)";
				else
					msg = "[" .. config.getGameservername() .. "]本线经验倍数：" .. battleexp .. "倍("..ExpUpTime[4] .. "+" .. ExpUpTime[1] .. "倍)";
				end
			end
		end
	end
	
	char.TalkToCli(charindex, -1, msg, 5);
	return ;
end

function FreeModeExp( charindex, getexp, addexp )
	--print("原始经验:"..getexp, "最终经验:"..addexp);
	local mymap = char.getInt(charindex,"地图号");
	for i=1,table.getn(NoExpMap) do
		if mymap == NoExpMap[i] then
			addexp = 0;
			break ;
		end
	end
	
	--周末经验加成
	local tmtime = other.atoi(os.time());
	local Tmweekday = other.atoi(os.date("%w",os.time()));
	local TmBase = 0;
	for i=1,table.getn(WeekDay) do
		if WeekDay[i][1] == Tmweekday then
			TmBase = WeekDay[i][2];
			break ;
		end
	end
	if TmBase > 0 then
		addexp = addexp + (getexp * TmBase);
		return addexp;
	end
	--周末经验加成结束
	
	--特殊时间段经验加成
	if TmBase <= 0 then
		if ExpUpTime[1] > 0 then
			if tmtime >= ExpUpTime[2] and tmtime <= ExpUpTime[3] then
				addexp = addexp + (getexp * ExpUpTime[1]);
				return addexp;
			end
		end
	end
	--特殊时间段经验加成结束
	return addexp;
end

function reload()
	mydata();
end

function mydata()
	--1经验倍数,2开始时间,3结束时间,4活动名
	ExpUpTime = {1,1546704000,1547222400,"新服经验活动"};--经验活动时间范围
	--星期经验双倍效果
	WeekDay = {{5,1}};
	NoExpMap = {40001,40002,40003,40004,40005,40006,40007,40008,40009,40010,40011};--不可获得经验的地图
end

function main()
	mydata();
	magic.addLUAListFunction("重读经验设置", "reload", "", 1, "[重读经验设置]");
end
