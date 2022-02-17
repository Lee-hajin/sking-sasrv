--此LUA是离线判断时间和扣除时间还有下线后记录上一次在线期间产生的个人信息!
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

function FreeCharLogout( charindex )
	--print("退出帐号");
	local token = "DELETE FROM CSA_MaxIp WHERE Account='" ..  char.getChar(charindex,"帐号").."';"
	sasql.query(token);
	return 1;
end


function mydata()

end

function main()
	mydata();
end
