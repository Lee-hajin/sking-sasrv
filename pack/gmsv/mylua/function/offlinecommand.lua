--离线战斗时调用的LUA
local battletime={};
function OffLineCommand( battleindex, charaindex, side)
	if char.getInt(charaindex, "类型") == 1 or  char.getInt(charaindex, "类型") == 53 then
		if battletime[charaindex] == nil then
			battletime[charaindex] = 0;
		end
		if os.time() < battletime[charaindex] then
			return;
		end
		battletime[charaindex] = os.time() + 9;
		if(char.getFlg(charaindex,"死亡") == 0) then
			if(char.getInt(charaindex, "HP") < (char.getWorkInt(charaindex, "最大HP") * 0.8)) then
				offline.Recovery(battleindex, charaindex,charaindex, 600);
			else
				local pindex = char.getCharPet(charaindex, char.getInt(charaindex, "骑宠"));
				if char.check(pindex) == 1 then
					if(char.getFlg(pindex,"死亡") == 0) then
						if(char.getInt(pindex, "HP") < (char.getWorkInt(pindex, "最大HP") * 0.8)) then
							offline.Recovery(battleindex, charaindex,charaindex, 600);
							return;
						end
					end
				end
				local pindex2 = char.getCharPet(charaindex, char.getInt(charaindex, "战宠"));
				if char.check(pindex2) == 1 then
					if(char.getFlg(pindex2,"死亡") == 0) then
						if(char.getInt(pindex2, "HP") < (char.getWorkInt(pindex2, "最大HP") * 0.8)) then
							offline.Recovery(battleindex, charaindex,pindex2, 600);
							return;
						end
					end
				end
				--默认攻击
				offline.Attack(battleindex, charaindex, side);
			end
		else
			offline.Attack(battleindex, charaindex, side);
			return;
		end
	else
		--当HP剩下50%时
		--if(char.getInt(charaindex, "HP") < (char.getWorkInt(charaindex, "最大HP") * 0.4)) then
			--加500血
		--	offline.Recovery(battleindex, charaindex, 2000)
		--else
			--默认攻击
			offline.Attack(battleindex, charaindex, side);
		--end
	end
end
