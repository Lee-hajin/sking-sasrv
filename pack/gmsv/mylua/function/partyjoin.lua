--组队按钮点击回调事件
function FreePartyJoin( meindex, toindex )
	if other.CallFunction("RePlayerPoint", "mylua/other/RePlayerPoint.lua", {meindex}) == 1 then
		char.TalkToCli(meindex, -1, "点数异常，已自动为你重置点数！", 6);
		char.TalkToCli(toindex, -1, "对方点数异常，已自动重置他的点数！", 6);
	end
	if other.CallFunction("RePlayerPoint", "mylua/other/RePlayerPoint.lua", {toindex}) == 1 then
		char.TalkToCli(toindex, -1, "点数异常，已自动为你重置点数！", 6);
		char.TalkToCli(meindex, -1, "对方点数异常，已自动重置他的点数！", 6);
	end
	
	local mapid1 = char.getInt(meindex, "地图号");
	for i = 1, table.getn(fmpkfloorid) do
		if mapid1 == fmpkfloorid[i] then
			if fmpkfloorPlayNum[i] == 1 then
				char.TalkToCli(meindex, -1, "该地图的族战只允许单人族战模式！", "黄色");
				return 0;
			end
			if char.getWorkInt(toindex, "组队") ~= 0 then
				local num = 0;
				for j = 1, 5 do
					local pindex = char.getWorkInt(toindex, "队员" .. j);
					if char.check(pindex) == 1 then
						num = num +1;
						if num >= fmpkfloorPlayNum[i] then
							char.TalkToCli(meindex, -1, "该地图的族战只允许" .. fmpkfloorPlayNum[i] .. "V族战模式！", "黄色");
							return 0;
						end
					end
				end
			end
			break
		end
	end
	return 1
end

function data()
	fmpkfloorid = {1042, 2032, 3032, 4032, 5032,6032,7032,8032,9032,7532};
	fmpkfloorPlayNum = {5,5,5,5,5,5,1,1,1,5};
end

function main()
	data()
end
