function GetEvent(charaindex,_data)
	local eventstr = other.getString(_data, " ", 1);
	if eventstr == "" then
		char.TalkToCli(charaindex, -1, "请输入要查询的旗标号，多个旗标号用|分隔", "红色");
		return ;
	end
	local CdKey = other.getString(_data, " ", 2);
	local toindex = charaindex;
	if CdKey ~= "" then
		toindex = -1;
		local maxplayer = char.getPlayerMaxNum() - 1;
		for i = 0, maxplayer do
			if char.check(i) == 1 then
				if char.getChar(i, "账号") ==  CdKey then
					toindex = i;
					break ;
				end
			end
		end
	end
	local token = "";
	if char.check(toindex) > 0 then
		for i=1,10 do
			local tmstr = other.getString(_data, "|", i);
			if tmstr ~= "" then
				local eventid = other.atoi(tmstr);
				print(eventid);
				if npc.Free(-1, toindex, "ENDEV="..eventid) == 1 then
					token = token .. "["..eventid..":完成]";
				elseif npc.Free(-1, toindex, "NOWEV="..eventid) == 1 then
					token = token .. "["..eventid..":已接]";
				else
					token = token .. "["..eventid..":未接]";
				end
			end
		end
	else
		token = "要查询的帐号不在线！";
	end
	char.TalkToCli(charaindex, -1, token, 6);
end


function mydata()
	
end

function main()
	mydata();
	magic.addLUAListFunction("查询旗标", "GetEvent", "", 3, " ");
end
