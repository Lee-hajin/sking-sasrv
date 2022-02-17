--地图传送条件
function FreeWarpMap( charaindex, floorid, x, y )
	if other.CallFunction("RePlayerPoint", "mylua/other/RePlayerPoint.lua", {charaindex}) == 1 then
		char.TalkToCli(charaindex, -1, "点数异常，已自动为你重置点数！", 6);
	end
	
	local index = -1;
	for i =1 , table.getn(fmpkfloorid) do 
		if fmpkfloorid[i] == floorid then
			index = i;
			break;
		end
	end
	if index > -1 then
		local macnum = 1;
		local mac = char.getWorkChar(charaindex,"MAC")
		local MaxPlayer = char.getPlayerMaxNum() - 1
		for i=0,MaxPlayer do
			if char.check(i) == 1 then
				if i ~= charindex then
					if char.getInt(i,"地图号") == fmpkfloorid[index] and mac == char.getWorkChar(i,"MAC") then
						macnum = macnum + 1;
						if macnum > fmpkfloorPlayNum[index] then
							char.TalkToCli(charaindex, -1, "该族战地图只能进"..fmpkfloorPlayNum[index].."号！", "黄色")
							return 0;
						end
					end
				end
			end
		end
	end
	

	--if floorid == 7000 or floorid == 60049 or floorid == 60050 or floorid == 60054 or floorid == 700 then
		--print("调试信息",npc.CheckEvent(charaindex,81));
		--if npc.CheckEvent(charaindex,81) == 0 then
			--char.TalkToCli(charaindex, -1, "您尚未完成金飞任务", "红色");
			--return 0;
		--end
	--end
	
	return 1
end

function data()
	fmpkfloorid = {1042, 2032, 3032, 4032, 5032};
	fmpkfloorPlayNum = {5,5,3,1,5};
end

function main()
	data()
end
