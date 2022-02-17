function DelRide(_charaindex, _data)--移除坐骑权限
	local rideid = other.atoi(other.getString(_data, " ", 1));
	local cdkey = other.getString(_data, " ", 2);
	local toindex = -1;
	if cdkey == "" then
		toindex = _charaindex;
	else
		local maxplayer = char.getPlayerMaxNum();
		for i = 0, maxplayer - 1 do
			if char.check(i) == 1 then
				local tmcdkey = char.getChar(i, "帐号");
				if cdkey == tmcdkey then
					toindex = i;
					break ;
				end
			end
		end
		if toindex <= -1 then
			char.TalkToCli(_charaindex, -1, "该帐号玩家不在线上！", "红色");
			return ;
		end
	end
	char.DelRide(toindex,rideid);
	if toindex ~= _charaindex then
		char.TalkToCli(_charaindex, -1, "移除玩家["..char.getChar(toindex,"名字").."] "..rideid.." 号坐骑权限成功！", "红色");
		char.TalkToCli(toindex, -1, "GM移除了你 "..rideid.." 号坐骑权限！", "红色");
	else
		char.TalkToCli(_charaindex, -1, "移除了"..rideid.."号坐骑权限成功！", "红色");
	end
	return 0;
end


function GiveRide(_charaindex, _data)--增加坐骑权限
	local rideid = other.atoi(other.getString(_data, " ", 1));
	local cdkey = other.getString(_data, " ", 2);
	local toindex = -1;
	if cdkey == "" then
		toindex = _charaindex;
	else
		local maxplayer = char.getPlayerMaxNum();
		for i = 0, maxplayer - 1 do
			if char.check(i) == 1 then
				local tmcdkey = char.getChar(i, "帐号");
				if cdkey == tmcdkey then
					toindex = i;
					break ;
				end
			end
		end
		if toindex <= -1 then
			char.TalkToCli(_charaindex, -1, "该帐号玩家不在线上！", "红色");
			return ;
		end
	end
	char.GiveRide(_charaindex,rideid);
	if toindex ~= _charaindex then
		char.TalkToCli(_charaindex, -1, "为玩家["..char.getChar(toindex,"名字").."]设置了 "..rideid.." 号坐骑权限成功！", "红色");
		char.TalkToCli(toindex, -1, "GM为你设置了 "..rideid.." 号坐骑权限！", "红色");
	else
		char.TalkToCli(_charaindex, -1, "设置了"..rideid.."号坐骑权限成功！", "红色");
	end
	return 0;
end


function main()
	magic.addLUAListFunction("移除坐骑", "DelRide", "", 3, " ");
	magic.addLUAListFunction("增加坐骑", "GiveRide", "", 3, " ");
end
