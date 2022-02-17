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

function RePlayerPoint(charaindex)
	--点数上限判断
	local mypoint = char.getInt(charaindex, "技能点");
	local vital = getIntPart(char.getInt(charaindex, "体力")  / 100);
	local str = getIntPart(char.getInt(charaindex, "腕力") / 100);
	local tgh = getIntPart(char.getInt(charaindex, "耐力") / 100);
	local dex = getIntPart(char.getInt(charaindex, "速度") / 100);
	local sum = vital + str + tgh + dex + mypoint;
	if sum > 629 then--最高点数
		local logstr = string.format( "%s|帐:%s|名:%s|点:%d 血:%d 攻:%d 防:%d 敏:%d 总:%d\n"
					,os.date("%Y-%m-%d %X",os.time())
					,char.getChar(charaindex, "账号")
					,char.getChar(charaindex, "名字")
					,mypoint,vital,str,tgh,dex,sum);
		local f = assert(io.open("./mylua/log/LvUpPoint/"..os.date("%Y%m%d",os.time())..".log", "a"));
		f:write( logstr);
		f:close();
		char.setInt(charaindex, "体力",1000);
		char.setInt(charaindex, "腕力",0);
		char.setInt(charaindex, "耐力",0);
		char.setInt(charaindex, "速度",0);
		char.setInt(charaindex, "技能点",619);
		char.complianceParameter(charaindex);
		char.sendStatusString(charaindex, "P");
		char.Skillupsend(charaindex);
		return 1;
	end
	return 0;
end


function mydata()
	
end

function main()
	mydata();
end
