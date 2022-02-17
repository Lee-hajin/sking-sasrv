--PK按钮点击回调事件
function FreeVsPlayer( meindex, toindex )
	--点数上限判断
	--meindex为点击人
	--toindex为面对人
	if other.CallFunction("RePlayerPoint", "mylua/other/RePlayerPoint.lua", {meindex}) == 1 then
		char.TalkToCli(meindex, -1, "点数异常，无法PK，已自动为你重置点数！", 6);
		char.TalkToCli(toindex, -1, "对方点数异常，无法PK，已自动重置他的点数！", 6);
		char.setFlg(meindex,"决斗",0);
		return 1;
	end
	parameter = {toindex};
	if other.CallFunction("RePlayerPoint", "mylua/other/RePlayerPoint.lua", parameter) == 1 then
		char.TalkToCli(toindex, -1, "点数异常，无法PK，已自动为你重置点数！", 6);
		char.TalkToCli(meindex, -1, "对方点数异常，无法PK，已自动重置他的点数！", 6);
		char.setFlg(toindex,"决斗",0);
		return 1;
	end
	fmpkpoint = {1042, 2032, 3032, 4032,5032,6032}
	for i = 1, table.getn(nopkfloor) do 
		if char.getInt(meindex, "地图号") == nopkfloor[i] then
			char.TalkToCli(meindex, -1, "该地图禁止私下PK！", "黄色");
			return 1;
		end
	end
		
	for i = 1, table.getn(fmpkpoint) do 
		if char.getInt(meindex, "地图号") == fmpkpoint[i] then
			if char.getWorkInt(toindex,"PK时间") + 120 > other.time() then
				char.TalkToCli(meindex, -1, "对方正在连点保护中，剩余时间：" .. char.getWorkInt(toindex,"PK时间") + 120 - other.time() .. "秒，对方如果移动或转向保护结束。", "随机色")
				return 1;
			end
		end
	end
	return 0;
end

function main()
	nopkfloor = {12345, 140};
	fmpkpoint = {1042, 2032, 3032, 4032,5032,6032,7032,8032,9032,7532};
end

