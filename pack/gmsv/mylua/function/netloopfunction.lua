function getIntPart(x)
    if x <= 0 then
       return math.ceil(x);
    end

    if math.ceil(x) == x then
       x = math.ceil(x);
    else
       x = math.ceil(x) - 1;
    end
    return x;
end

--此LUA为循环函数，每分钟执行一次
function NetLoopFunction()
	local playernum = 0;
	local maxplayer = char.getPlayerMaxNum();
	local nowday = other.atoi(os.date("%d",os.time()));
	for i = 0, maxplayer - 1 do
		if char.check(i) == 1 then
			local floorid = char.getInt(i, "地图号");
			--离线时间判断
			if char.getWorkInt(i, "离线") == 1 then
				if char.getWorkInt(i, "摆摊") == -1 then
					char.setInt(i,"离线时间",char.getInt(i,"离线时间") - 1);
					if char.getInt(i,"离线时间") <= 0 then
						char.logou(i);
					end
				end
			end
		end
	end
	if math.mod(looptime , 10) == 0 then--每10分钟执行一次
		for i = 0, maxplayer - 1 do
			if char.check(i) == 1 then
				if char.getWorkInt(i,"离线") < 1 then 
					if char.getInt(i,"转数") > 0 then
						char.setInt(i,"活力",char.getInt(i,"活力")+1);
					elseif char.getInt(i,"等级") >= 120 then
						char.setInt(i,"活力",char.getInt(i,"活力")+1);
					end
				end
			end
		end
	end
	--公告模块
	if math.mod(looptime , 30) == 0 then--每30分钟执行一次
		token = message[math.random(table.getn(message))]
		char.talkToServer(-1, token, 4);
		for i = 0, maxplayer - 1 do
			if char.check(i) == 1 then
				lssproto.dengon(i, token, 4, 5);
			end
		end
	end
	looptime = looptime + 1;
	--if looptime >= 3120 then
		--looptime = 0;
	--end
end


function mydata()
	message = {"胶沛俊捞瘤 其剐府辑滚俊 坷脚 巴阑 券康钦聪促！ 榴芭款 矫埃 登技夸！"
				,"款康磊绰 蜡历狼 酒捞叼,菩胶况靛甫 汞瘤 臼嚼聪促."
				,"皋农肺/飘饭捞呈 Log啊 角矫埃 馆康 吝涝聪促。"
				,"泅陛芭贰 蜡档绰 力犁 措惑涝聪促！惯斑 矫 力焊秦林矫扁 官而聪促."
				,"款康磊绰 阿 葱匙烙栏肺 劝悼窍聪 荤莫俊 林狼窍矫扁 官而聪促。"
				,"辑滚巩狼唱 龙巩荤亲篮 权其捞瘤甫 捞侩秦林矫扁 官而聪促。"
				,"蜡历甸苞 家烹窍绰 辑滚 其剐府俊 坷脚 巴阑 券康钦聪促！"
				,"泅陛芭贰 蜡档绰 力犁 措惑涝聪促！惯斑 矫 力焊秦林矫扁 官而聪促."
				,"泅陛芭贰 蜡档绰 力犁 措惑涝聪促！惯斑 矫 力焊秦林矫扁 官而聪促."
				--,"款康磊绰 蜡历狼 酒捞叼,菩胶况靛甫 汞瘤 臼嚼聪促."
				,"皋农肺/飘饭捞呈 Log啊 角矫埃 馆康 吝涝聪促。"
				,"胶沛俊捞瘤 其剐府辑滚俊 坷脚 巴阑 券康钦聪促！ 榴芭款 矫埃 登技夸！"
				};
	NetLoopFunction();
end

function setnetloop(charaindex, data)
	looptime = other.atoi(data);
	
end

function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.TalkToCli(_charaindex, -1, "重读NPC[netloop]完成", 6);
	end
end

function main()
	reloadnpcNum =0;
	reloadnpcTime = os.time() + 30;
	looptime = 0;
	mydata();
	magic.addLUAListFunction("setnetloop", "setnetloop", "", 3, "[setnetloop 分钟]");
	magic.addLUAListFunction("reloadnetloop", "reload", "", 1, "[reloadnetloop]");
end

