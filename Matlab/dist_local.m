%distance locale
function d = dist_local( x,y )

    d = 0;

    for i = 1:length(x)
        d = d + (x(i)-y(i))*(x(i)-y(i));
    end

end
