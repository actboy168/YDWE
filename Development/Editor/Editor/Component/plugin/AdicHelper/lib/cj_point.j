//Edit By Warft_TigerCN

struct Point {

    float x, y, distance, angle;
    
    static thistype create(float x, float y) {
        thistype this = thistype.allocate();
        this.x = x;
        this.y = y;
        return this;
    }
    
    void changeValue(float x, float y) {
        .x = x;
        .y = y;
    }
    
    float getDistance(thistype other) {
        .distance = SquareRoot((other.x - .x) * (other.x - .x) + (other.y - .y) * (other.y - .y));
        return .distance;
    }
    
    float getAngle(thistype other) {
        .angle = 57.296 * Atan2(other.y - .y, other.x - .x);
        return .angle;
    }
    
    void setUnitCoordinate(unit u) {
        SetUnitX(u, .x);
        SetUnitY(u, .y);
    }
    
}