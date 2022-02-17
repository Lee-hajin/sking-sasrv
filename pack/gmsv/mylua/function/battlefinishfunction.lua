function checkEmptItemNum(charaindex)
	EmptyItemNum = 0
	for i = 9, 23 do
		if char.getItemIndex(charaindex, i) == -1 then
			EmptyItemNum = EmptyItemNum + 1
		end
	end
	return EmptyItemNum
end

--战斗结束回调函数，battletime为战斗时间,battleturn战斗回合，win战斗胜利或失败
function BattleFinishFunction( charaindex, battletime, battleturn, win )
	--修正战后依然沉默
	if char.getWorkInt(charaindex,50) == 1 then
		char.setWorkInt(charaindex,50,0)
	end
	if char.getInt(charaindex, "类型") == 1 then
		if char.getInt(charaindex, "转数") == 5 then
			if win == 1 then
				local floorid = char.getInt(charaindex, "地图号")
				if config.getGameservername() == "族战活动二线" then
					for i = 1, table.getn(fmpkpoint) do
						if fmpkpoint[i] == floorid then
							
							return;
						end
					end
				end
			end
		end
	end
end

function battleinfo(charaindex, data)
	char.talkToAllServer("<PK互动快讯>" .. data .. "在("..  config.getGameservername()  .. ")" .. map.getFloorName(777) .. "PK幸运获得50点活力",5)
end

function data()
	fmpkpoint = {1042, 2032, 3032, 4032, 5032};
	fmpkItem = {21440,5};
	--130地图
	itemlist = {1};
	--12345地图
	pkitemlist = {1};

end

function main()
	data()
	magic.addLUAListFunction("battleinfo", "battleinfo", "", 3, "测试专用命令")
end
