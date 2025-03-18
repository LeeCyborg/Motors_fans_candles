#pragma once

class MotLerp
{
public:
    MotLerp() = default;

    bool configure(int min_val, int max_val, int rate, int step_size);
    void reset(int target);
    void set_target(int target);
    int evaluate();

    int value() const {
        return m_current;
    }

private:
    int m_current = 1000;
    int m_target = 1000;

    int m_min_val = 1000;
    int m_max_val = 2000;
    int m_step = 10;
};

inline bool MotLerp::configure(const int min_val, const int max_val,
                               const int val_per_sec, const int step_size_millis)
{
    bool result = true;
    if ((min_val > max_val) ||
        (val_per_sec < 0) ||
        (step_size_millis <= 0))
    {
        // bad input
        result = false;
    }
    else
    {
        const int step = (val_per_sec * step_size_millis) / 1000;
        if (step == 0)
        {
            // rate is too slow given step size
            result = false;
        }
        else
        {
            m_step = step;
            m_min_val = min_val;
            m_max_val = max_val;
        }
    }

    return result;
}

void MotLerp::reset(const int target)
{
    set_target(target);
    m_current = m_target;
}

void MotLerp::set_target(const int target)
{
    if (target < m_min_val)
    {
        m_target = m_min_val;
    }
    else if (target > m_max_val)
    {
        m_target = m_max_val;
    }
    else
    {
        m_target = target;
    }
}

int MotLerp::evaluate()
{
    if (m_target > m_current)
    {
        m_current = m_current + m_step;
        if (m_current > m_target)
        {
            m_current = m_target;
        }
    }
    else if (m_target < m_current)
    {
        m_current = m_current - m_step;
        if (m_current < m_target)
        {
            m_current = m_target;
        }
    }
    return m_current;
}
